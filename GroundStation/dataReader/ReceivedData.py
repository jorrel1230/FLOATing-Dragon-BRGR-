from pathlib import Path
from datetime import datetime

class ReceivedData:
    global latest_data
    global logfile

    def __init__(self):
        self.latest_data = {}
        i = 0
        while True:  # check if file already exists
            filename = 'datalinkLog' + str(i) + '.txt'
            path = Path('C:\\Users\\joeyf\\PycharmProjects\\dataReader' + filename)
            if path.is_file():
                i = i + 1
            else:
                i
                break
        # Creating a file
        filename = 'datalinkLog' + str(i) + '.txt'
        self.logfile = open('C:\\Users\\joeyf\\PycharmProjects\\dataReader' + filename, "w")



    def add(self, data_name, data_value):
        if data_name in self.latest_data:
            self.latest_data[data_name] = data_value
        else:
            self.latest_data.update({data_name: data_value})

        now = datetime.now()
        current_time = now.today()
        #print('C:\\Users\\Adin\\Desktop\\BRGR\\Code\\DatalinkLogging\\' + filename)
        self.logfile.write(str(current_time) + ': ' + data_name + ': ' + str(data_value) + '\n')


    def get(self, data_name):
        if data_name in self.latest_data:
            return self.latest_data[data_name]
        else:
            return None

    def get_all(self):
        return self.latest_data
# bruh = ReceivedData()
# bruh.add('lol', 5)
