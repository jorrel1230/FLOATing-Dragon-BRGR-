def int_to_percent24(data, maybe_negative=True):
    if not maybe_negative:
        data_percent = (data[0] << 16) + (data[1] << 8) + data[2]
        return data_percent / 16777215.0

    if data[0] & b'\x80'[0] == b'\x80'[0]:
        is_negative = True
    else:
        is_negative = False
    first_byte = data[0] & b'\x7f'[0]
    data_percent = (first_byte << 16) + (data[1] << 8) + data[2]

    if is_negative:
        data_percent = data_percent - 8388608.0
    data_percent = data_percent / 8388608.0

    return data_percent


def int_to_percent16(data, maybe_negative=True):
    if not maybe_negative:
        data_percent = (data[0] << 8) + data[1]
        return data_percent / 65536.0

    if data[0] & b'\x80'[0] == b'\x80'[0]:
        is_negative = True
    else:
        is_negative = False
    first_byte = data[0] & b'\x7f'[0]
    data_percent = (first_byte << 8) + data[1]

    if is_negative:
        data_percent = data_percent - (32768.0)
    data_percent = data_percent / (32768.0)
    return data_percent


def get_data_attributes(datatype, requested_attribute='Bytes'):
    datatype_definitions = {
        1:  {'Name': "GPS LAT", 'Bytes': 3},
        2:  {'Name': "GPS LON", 'Bytes': 3},
        3:  {'Name': "GPS ALT", 'Bytes': 2},
        4:  {'Name': "GPS SPD", 'Bytes': 2},
        5:  {'Name': "GPS ANG", 'Bytes': 2},
        6:  {'Name': "BAR TMP", 'Bytes': 1},
        7:  {'Name': "BAR ALT", 'Bytes': 2},
        8:  {'Name': "BAR KPA", 'Bytes': 2},
        9:  {'Name': "PTT KPA", 'Bytes': 2},
        10: {'Name': "PTT SPD", 'Bytes': 2},
        11: {'Name': "IMU ORX", 'Bytes': 2},
        12: {'Name': "IMU ORY", 'Bytes': 2},
        13: {'Name': "IMU ORZ", 'Bytes': 2},
        14: {'Name': "IMU ACX", 'Bytes': 2},
        15: {'Name': "IMU ACY", 'Bytes': 2},
        16: {'Name': "IMU ACZ", 'Bytes': 2},
        17: {'Name': "IMU RTX", 'Bytes': 2},
        18: {'Name': "IMU RTY", 'Bytes': 2},
        19: {'Name': "IMU RTZ", 'Bytes': 2},
        20: {'Name': "IMU MGX", 'Bytes': 2},
        21: {'Name': "IMU MGY", 'Bytes': 2},
        22: {'Name': "IMU MGZ", 'Bytes': 2},
        23: {'Name': "FST SRV", 'Bytes': 1},
        24: {'Name': "SLW SRV", 'Bytes': 1},
        25: {'Name': "ELP TME", 'Bytes': 3},
        26: {'Name': "CPU TMP", 'Bytes': 1},
        27: {'Name': "GPS TME", 'Bytes': 3},
        28: {'Name': "GPS DTE", 'Bytes': 2},
        29: {'Name': "IMU LNX", 'Bytes': 2},
        30: {'Name': "IMU LNY", 'Bytes': 2},
        31: {'Name': "IMU LNZ", 'Bytes': 2},
        32: {'Name': "ERR BOL", 'Bytes': 1}
    }

    try:
        return datatype_definitions[datatype][requested_attribute]
    except:
        return None


def interpret_data(data, name):
    match name:
        case 'GPS LAT':
            return 90 * int_to_percent24(data, True)
        case 'GPS LON':
            return 180 * int_to_percent24(data, True)
        case 'GPS ALT':
            return (data[0] << 8) + data[1]
        case 'GPS SPD':
            return 655.35 * int_to_percent16(data, False)
        case 'GPS ANG':
            return 360 * int_to_percent16(data, False)
        case 'GPS TMP':
            return -80 + data[0]
        case 'BAR TMP':
            return -80 + data[0]
        case 'BAR ALT':
            return (data[0] << 8) + data[1]
        case 'BAR KPA':
            return 120 * int_to_percent16(data, False)
        case 'PTT KPA':
            return 6.89476 * int_to_percent16(data, True)
        case 'PTT SPD':
            return 655.35 * int_to_percent16(data, False)
        case 'IMU ORX':
            return 360 * int_to_percent16(data, False)
        case 'IMU ORY':
            return 90 * int_to_percent16(data, True)
        case 'IMU ORZ':
            return 180 * int_to_percent16(data, True)
        case 'IMU ACX':
            return 16 * int_to_percent16(data, True)
        case 'IMU ACY':
            return 16 * int_to_percent16(data, True)
        case 'IMU ACZ':
            return 16 * int_to_percent16(data, True)
        case 'IMU RTX':
            return 2000 * int_to_percent16(data, True)
        case 'IMU RTY':
            return 2000 * int_to_percent16(data, True)
        case 'IMU RTZ':
            return 2000 * int_to_percent16(data, True)
        case 'IMU MGX':
            return 1300 * int_to_percent16(data, True)
        case 'IMU MGY':
            return 1300 * int_to_percent16(data, True)
        case 'IMU MGZ':
            return 1300 * int_to_percent16(data, True)
        case 'FST SRV':
            return data[0]
        case 'SLW SRV':
            return data[0]
        case 'ELP TME':
            return ((data[0] << 16) + (data[1] << 8) + data[2]) * 10
        case 'CPU TMP':
            return -80 + data[0]
        case 'GPS TME':
            seconds_since_midnight = ((data[0] << 16) + (data[1] << 8) + data[2]) / 100.0
            hours = int(seconds_since_midnight / 3600)
            minutes = int((seconds_since_midnight % 3600) / 60)
            seconds = int(seconds_since_midnight % 60)
            msec = int((seconds_since_midnight % 1) * 1000)
            return str(hours) + ':' + str(minutes) + ':' + str(seconds) + '.' + str(msec)
        case 'GPS DTE':
            day = (data[0] & 248) >> 3  # first 5 bits
            month = ((data[0] & 7) << 1) + ((data[1] & 128) >> 7)  # next 4 bits
            year = (data[1] & 127) + 1970  # last 7 bits, since 1970
            return str(month) + '/' + str(day) + '/' + str(year)
        case 'IMU LNX':
            return 16 * int_to_percent16(data, True)
        case 'IMU LNY':
            return 16 * int_to_percent16(data, True)
        case 'IMU LNZ':
            return 16 * int_to_percent16(data, True)
        case 'ERR BOL':
            return data[0]


class DataInterpreter:
    pass
# print(get_data_attributes(1, 'Name'))