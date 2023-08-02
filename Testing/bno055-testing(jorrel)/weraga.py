import serial
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import threading

def read_serial(serial_port, data_buffer):
    try:
        with serial.Serial(serial_port, 9600) as ser:
            while True:
                line = ser.readline().decode('utf-8').strip()
                if line:
                    try:
                        x, y, z = map(float, line.split(','))
                        data_buffer['x'].append(x)
                        data_buffer['y'].append(y)
                        data_buffer['z'].append(z)
                    except ValueError:
                        print(f"Invalid data format: {line}")
    except serial.SerialException as e:
        print(f"Error reading serial port: {e}")
        return

def plot_real_time(data_buffer):
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')
    ax.set_title('Real-Time 3D Plot')

    while True:
        ax.cla()
        ax.scatter(data_buffer['x'], data_buffer['y'], data_buffer['z'], s=5)
        plt.pause(0.01)

def main():
    serial_port = "/dev/cu.usbmodem137721801"  # Replace this with your actual serial port name (e.g., "/dev/ttyUSB0" on Linux)
    data_buffer = {'x': [], 'y': [], 'z': []}

    serial_thread = threading.Thread(target=read_serial, args=(serial_port, data_buffer))
    serial_thread.daemon = True
    serial_thread.start()

    plot_real_time(data_buffer)

if __name__ == "__main__":
    main()
