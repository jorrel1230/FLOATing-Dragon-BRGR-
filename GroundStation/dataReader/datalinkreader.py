import serial
import DataInterpreter
import queue
from ReceivedData import ReceivedData


def crc8(input_bytes):  # to you, its magic
    numbytes = len(input_bytes)
    sht75_crc_table = [
        0,   49,  98,  83,  196, 245, 166, 151, 185, 136, 219, 234, 125, 76,  31,  46,
        67,  114, 33,  16,  135, 182, 229, 212, 250, 203, 152, 169, 62,  15,  92,  109,
        134, 183, 228, 213, 66,  115, 32,  17,  63,  14,  93,  108, 251, 202, 153, 168,
        197, 244, 167, 150, 1,   48,  99,  82,  124, 77,  30,  47,  184, 137, 218, 235,
        61,  12,  95,  110, 249, 200, 155, 170, 132, 181, 230, 215, 64,  113, 34,  19,
        126, 79,  28,  45,  186, 139, 216, 233, 199, 246, 165, 148, 3,   50,  97,  80,
        187, 138, 217, 232, 127, 78,  29,  44,  2,   51,  96,  81,  198, 247, 164, 149,
        248, 201, 154, 171, 60,  13,  94,  111, 65,  112, 35,  18,  133, 180, 231, 214,
        122, 75,  24,  41,  190, 143, 220, 237, 195, 242, 161, 144, 7,   54,  101, 84,
        57,  8,   91,  106, 253, 204, 159, 174, 128, 177, 226, 211, 68,  117, 38,  23,
        252, 205, 158, 175, 56,  9,   90,  107, 69,  116, 39,  22,  129, 176, 227, 210,
        191, 142, 221, 236, 123, 74,  25,  40,  6,   55,  100, 85,  194, 243, 160, 145,
        71,  118, 37,  20,  131, 178, 225, 208, 254, 207, 156, 173, 58,  11,  88,  105,
        4,   53,  102, 87,  192, 241, 162, 147, 189, 140, 223, 238, 121, 72,  27,  42,
        193, 240, 163, 146, 5,   52,  103, 86,  120, 73,  26,  43,  188, 141, 222, 239,
        130, 179, 224, 209, 70,  119, 36,  21,  59,  10,  89,  104, 255, 206, 157, 172
    ]
    crc = 0
    for a in range(0, numbytes):
        crc = sht75_crc_table[input_bytes[a] ^ crc]
    return crc


# returns data if found (or none if not) and the amount to increment printer

# initialize serial port
ser = serial.Serial('COM6                                ', 115200, timeout=0.1)
# data storage thing constructor
newData = ReceivedData()

# queue used for incoming data
data_queue = queue.Queue(maxsize=100)


# active checking data, does not exceed the 5 bytes
checking_data = []

validData = None
empty = False  # if read queue is empty
while True:

    # if the big queue gets too low, refill
    if data_queue.qsize() < 10:
        try:
            incomingData = list(ser.read(70))  # make sure this is decently less than the max size
            for i in range(0, len(incomingData)):
                data_queue.put(incomingData[i])
            incomingData.clear()
        except serial.SerialTimeoutException:  # may not always get data
            pass

    # if something's in checking data check it before messing
    if len(checking_data) == 0:
        try:
            checking_data.append(data_queue.get(timeout=0.01))
            empty = False
        except queue.Empty:
            empty = True
    # checks if anything is ready
    if len(checking_data) > 0:
        data_length = DataInterpreter.get_data_attributes(checking_data[0])
    else:
        data_length = None

    # remove the first item if it definitely isn't data
    if data_length is None:
        try:
            checking_data.pop(0)
        except IndexError:
            checking_data.clear()
    else:  # might be valid, check crc
        for i in range(len(checking_data), data_length + 2):  # only fill to expected packet length
            try:
                checking_data.append(data_queue.get(timeout=0.01))
                empty = False
            except queue.Empty:
                empty = True

        # if true data is valid
        if crc8(bytes(checking_data)) == 0 and len(checking_data) > 0:
            validData = checking_data.copy()
            checking_data.clear()
        elif empty:  # might get the rest of the data in a second, don't lose it
            pass
        else:  # not getting the data, lose the first byte
            checking_data.pop(0)
            validData = []

    # interprets and saves latest data
    if validData is not None and len(validData) > 0:
        dataType = DataInterpreter.get_data_attributes(validData[0], 'Name')
        processed_data = DataInterpreter.interpret_data(validData[1:len(validData)-1], dataType)
        print(dataType + ' ' + str(processed_data))

        newData.add(dataType, processed_data)
        validData.clear()
        validData = []

