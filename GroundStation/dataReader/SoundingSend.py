import csv
import os
import time

import serial

fhand = open("soundings\predict_01_sound.txt", 'r')

csvfile = open('soundings\predict_01_sound.csv', 'w')
csvwriter = csv.writer(csvfile)

Lines = fhand.readlines()
fhand.close()

for line in Lines:
    values = line.split()
    if len(values) > 0:
        if values[0].isnumeric():
            data = [int(x) for x in values]
            csvwriter.writerow(data)

csvfile.close()

# initialize serial port
ser = serial.Serial('COM11', 115200, timeout=0.1)

file_stats = os.stat("soundings\predict_01_sound.csv")

integer_val = file_stats.st_size

filename = "predict_01_sound.csv"

i = 162
ser.write(i.to_bytes(1, 'big'))
time.sleep(1)
ser.write(len(filename).to_bytes(1, 'big'))
time.sleep(2)
ser.write(bytes(filename, 'utf-8'))
time.sleep(2)
ser.write(integer_val.to_bytes(2, 'big'))
time.sleep(3)
ser.write((open("soundings\predict_01_sound.csv", "rb").read()))


