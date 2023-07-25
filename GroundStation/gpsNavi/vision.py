import math
import cv2
import tkinter as tk
from PIL import ImageTk, Image
import numpy as np

root = tk.Tk()
root.title("Synthetic Vision System")
video_label = tk.Label(root)
video_label.pack()

# constants
screen_width = 1920
screen_height = 1080

THICKNESS = 2

# Constants for altitude and speed limits
ALTITUDE_RANGE = 500
SPEED_RANGE = 50
BEARING_RANGE = 60

# Line Dividers
alt_line_div = 250
vel_line_div = 20
bear_line_div = 30

# Bar Constant Colors
GREEN = (0, 255, 0)   # Green
RED = (0, 0, 255)   # Red
BLACK = (0, 0, 0)  # Black
PINK = (214, 0, 214)

# Bar percents
BAR_FROM_SIDE = 0.25
BAR_FROM_TOP = 0.3
BAR_PERC = 0.05

BEARING_BAR_WIDE = 1.6
BEARING_BAR_HEIGHT = 0.6

ARROW_PERC = 0.05

FONT = cv2.FONT_HERSHEY_SIMPLEX
FONT_SCALE = 1


globALT = 650
globSPD = 40
globVEL = [0, 0, 0]
globORT = [0, 1, 0]
globBEAR = 0
globTIME = 0

CROSSHAIR_RADIUS = 20


def show_frame():
    # Capture a frame from the video feed
    ret, frame = cap.read()
    frame = cv2.resize(frame, (screen_width, screen_height))

    # Get the latest sensor data from the glider
    sensor_data = get_sensor_data()

    # add green bars
    frame = add_green_bars(frame, sensor_data[0], sensor_data[1])
    # add prograde marker
    frame = add_prograde(frame, sensor_data[3])
    # add crosshair
    frame = add_crosshair(frame)
    # add bearing
    frame = add_bearing(frame, sensor_data[4])
    # add extras
    frame = add_extras(frame)

    # Convert the frame to RGB format and create an ImageTk object
    frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    img = Image.fromarray(frame_rgb)
    img_tk = ImageTk.PhotoImage(image=img)

    # Update the video feed display
    video_label.img_tk = img_tk
    video_label.configure(image=img_tk)

    # Schedule the next frame update
    video_label.after(10, show_frame)


def get_sensor_data():
    # Your code to fetch the sensor data from the glider goes here
    # Replace the example with your actual implementation
    sensor_data = [0, 0, [], [], 0]
    # Altitude, Speed, Orientation, Velocity, Bearing


    return sensor_data


def linear_conversion(old_value, old_min, old_max, new_max, new_min):
    return ((old_value - old_min) / (old_max - old_min)) * (new_max - new_min) + new_min


def get_divisible_numbers(x, y, z):
    start = x + (z - x % z) % z
    end = y - y % z
    divisible_numbers = list(range(start, end + 1, z))
    return divisible_numbers


def rad2deg(radians):
    return radians * 180 / math.pi


def deg2rad(deg):
    return deg / 180 * math.pi


def proj_2d(dir_vector, vel_vector):
    # Projection of velocity vector onto direction vector
    vel_proj = np.array(dir_vector) * np.dot(dir_vector, vel_vector) / np.dot(dir_vector, dir_vector)

    vel_2 = np.add(vel_vector, -1 * vel_proj)

    offset_vector = np.array([
        vel_2[0],
        - dir_vector[1]*vel_2[0] + dir_vector[0]*vel_2[1],
        - dir_vector[2]*vel_2[0] + dir_vector[0]*vel_2[2]
    ])

    print(dir_vector)
    print(vel_vector)
    print(vel_2)
    print(offset_vector)
    print("------")
    return offset_vector


def add_green_bars(frame, altitude, speed):

    screen_alt_min = int(screen_height * BAR_FROM_TOP)
    screen_alt_max = int(screen_height * (1-BAR_FROM_TOP))

    screen_alt_width1 = int(screen_width * (1 - BAR_FROM_SIDE - BAR_PERC))
    screen_alt_width2 = int(screen_width * (1 - BAR_FROM_SIDE))

    start_point_alt = (screen_alt_width1, screen_alt_min)
    end_point_alt = (screen_alt_width2, screen_alt_max)
    frame = cv2.rectangle(frame, start_point_alt, end_point_alt, GREEN, THICKNESS)
    middle_height = int(screen_height/2)

    adj_screen_alt_width2 = screen_alt_width2 + int(screen_width * ARROW_PERC)
    frame = cv2.putText(frame, "ALT", start_point_alt, FONT, FONT_SCALE, RED, THICKNESS)
    frame = cv2.arrowedLine(frame, (adj_screen_alt_width2, middle_height),
                            (screen_alt_width2, middle_height), RED, THICKNESS)
    frame = cv2.putText(frame, str(altitude), (adj_screen_alt_width2, middle_height),
                        FONT, FONT_SCALE, RED, THICKNESS)

    altitude_min = altitude-ALTITUDE_RANGE
    altitude_max = altitude+ALTITUDE_RANGE

    alt_div_arr = get_divisible_numbers(altitude_min, altitude_max, alt_line_div)

    for i in alt_div_arr:
        offset = int(linear_conversion(i, altitude_min, altitude_max, screen_alt_min, screen_alt_max))
        frame = cv2.line(frame, (screen_alt_width1, offset), (screen_alt_width2, offset), GREEN, THICKNESS)
        frame = cv2.putText(frame, str(i), (screen_alt_width2, offset), FONT, FONT_SCALE, GREEN, THICKNESS)

    screen_vel_max = int(screen_height * (1-BAR_FROM_TOP))
    screen_vel_min = int(screen_height * BAR_FROM_TOP)

    screen_vel_width1 = int(screen_width * BAR_FROM_SIDE)
    screen_vel_width2 = int(screen_width * (BAR_FROM_SIDE + BAR_PERC))

    start_point_vel = (screen_vel_width2, screen_vel_max)
    end_point_vel = (screen_vel_width1, screen_vel_min)
    frame = cv2.rectangle(frame, start_point_vel, end_point_vel, GREEN, THICKNESS)

    adj_screen_vel_width2 = screen_vel_width2 + int(screen_width * ARROW_PERC)
    frame = cv2.putText(frame, "VEL", end_point_vel, FONT, FONT_SCALE, RED, THICKNESS)
    frame = cv2.arrowedLine(frame, (adj_screen_vel_width2, middle_height),
                            (screen_vel_width2, middle_height), RED, THICKNESS)
    frame = cv2.putText(frame, str(speed), (adj_screen_vel_width2, middle_height),
                        FONT, FONT_SCALE, RED, THICKNESS)

    velocity_min = speed - SPEED_RANGE
    velocity_max = speed + SPEED_RANGE

    vel_div_arr = get_divisible_numbers(velocity_min, velocity_max, vel_line_div)

    for i in vel_div_arr:
        offset = int(linear_conversion(i, velocity_min, velocity_max, screen_vel_min, screen_vel_max))
        frame = cv2.line(frame, (screen_vel_width1, offset), (screen_vel_width2, offset), GREEN, THICKNESS)
        frame = cv2.putText(frame, str(i), (screen_vel_width2, offset), FONT, FONT_SCALE, GREEN, THICKNESS)

    return frame


def add_prograde(frame, rel_vel_vector):

    center_point = [int(screen_width/2 + rel_vel_vector[2]),
                    int(screen_height/2 - rel_vel_vector[1])]

    frame = cv2.circle(frame, center_point, CROSSHAIR_RADIUS, RED, THICKNESS)
    line_len = CROSSHAIR_RADIUS * 2
    frame = cv2.line(frame, (int(center_point[0] + line_len), center_point[1]),
                     (int(center_point[0] - line_len), center_point[1]), RED, THICKNESS)
    frame = cv2.line(frame, center_point,
                     (center_point[0], int(center_point[1] - line_len)), RED, THICKNESS)
    return frame


def add_crosshair(frame):
    center_point = (int(screen_width / 2), int(screen_height / 2))
    line_len = CROSSHAIR_RADIUS * 1
    frame = cv2.line(frame, (int(screen_width / 2 + line_len), int(screen_height / 2)),
                     (int(screen_width / 2 - line_len), int(screen_height / 2)), PINK, THICKNESS)
    frame = cv2.line(frame, (int(screen_width / 2), int(screen_height / 2 + line_len)),
                     (int(screen_width / 2), int(screen_height / 2 - line_len)), PINK, THICKNESS)
    return frame


def add_bearing(frame, bearing):
    screen_bear_min = int(screen_width * BEARING_BAR_WIDE * BAR_FROM_SIDE)
    screen_bear_max = int(screen_width * (1 - BEARING_BAR_WIDE * BAR_FROM_SIDE))

    screen_bear_height1 = int(screen_height * BEARING_BAR_HEIGHT * BAR_FROM_TOP)
    screen_bear_height2 = int(screen_height * (BEARING_BAR_HEIGHT * BAR_FROM_TOP + BAR_PERC))

    start_point_bear = (screen_bear_min, screen_bear_height1)
    end_point_bear = (screen_bear_max, screen_bear_height2)
    frame = cv2.rectangle(frame, start_point_bear, end_point_bear, GREEN, THICKNESS)
    ######

    middle_width = int(screen_width / 2)

    adj_screen_bear_height2 = screen_bear_height2 + int(screen_height * ARROW_PERC)
    frame = cv2.arrowedLine(frame, (middle_width, adj_screen_bear_height2),
                            (middle_width, screen_bear_height2), RED, THICKNESS)
    frame = cv2.putText(frame, str(bearing), (middle_width, adj_screen_bear_height2),
                        FONT, FONT_SCALE, RED, THICKNESS)

    bearing_min = bearing - BEARING_RANGE
    bearing_max = bearing + BEARING_RANGE

    bear_div_arr = get_divisible_numbers(bearing_min, bearing_max, bear_line_div)

    for i in bear_div_arr:
        offset = int(linear_conversion(i, bearing_min, bearing_max, screen_bear_max, screen_bear_min))
        if i < 0:
            i = i + 360
        elif i >= 360:
            i = i - 360

        match i:
            case 0:
                i = 'N'
            case 90:
                i = 'E'
            case 180:
                i = 'S'
            case 270:
                i = 'W'
        frame = cv2.line(frame, (offset, screen_bear_height1), (offset, screen_bear_height2), GREEN, THICKNESS)
        frame = cv2.putText(frame, str(i),
                            (int(offset - 0.035*BAR_FROM_SIDE * screen_width),
                             int(screen_bear_height1 - (BAR_PERC * 0.1 * screen_height))),
                            FONT, FONT_SCALE, GREEN, THICKNESS)

    return frame

def add_extras(frame):
    frame = cv2.putText(frame, str(i),
                        (int(offset - 0.035 * BAR_FROM_SIDE * screen_width),
                         int(screen_bear_height1 - (BAR_PERC * 0.1 * screen_height))),
                        FONT, FONT_SCALE, GREEN, THICKNESS)
    return frame


# Start:
# Specify the video source (0 for the default camera)
cap = cv2.VideoCapture(0)
# Start displaying the frames
show_frame()
# Run the Tkinter event loop
root.mainloop()
