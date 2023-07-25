import json

gpsDataArr = [0, 1, 2]
accDataArr = [3, 4, 5]

all_data = {
    "gps": gpsDataArr,
    "acc": accDataArr
}
json_dump = json.dumps(all_data)

# Write the JSON data to a file
with open('datalink/shared_array.json', 'w') as file:
    file.write(json_dump)
