# Untitled - By: Y8314 - Tue Jul 8 2025 b站

import sensor, image, time
from pyb import UART
import math

# Color threshold for green (adjust as needed)
green_threshold = (20, 120, -128, -10, -50, 50)  # LAB color space for green

# Initialize the camera sensor
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time=2000)
sensor.set_auto_gain(False)  # 必须关闭自动增益以进行颜色跟踪
sensor.set_auto_whitebal(False)  # 必须关闭自动白平衡以进行颜色跟踪
clock = time.clock()


uart = UART(2, 115200, timeout_char=200)
uart.init(115200, bits=8, parity=None, stop=1)  # init with given parameters

# Image center coordinates
img_center_x = sensor.width() // 2  # Image center X
img_center_y = sensor.height() // 2  # Image center Y

# PID parameters
Kp = 0.6  # Proportional gain
Ki = 0.03 # Integral gain 0.01
Kd = 0.2  # Derivative gain 0.1

# PID variables
last_error_x = 0
integral_x = 0
last_error_y = 0
integral_y = 0


Servo_dx = 0
Servo_dy = 0
Servo_data = [0,0];
# Function to find the largest blob


# HEADER = b'\xFF'  # 包头
# FOOTER = b'\xFE'  # 包尾
HEADER = bytearray([0xb3, 0xb3])  # 帧头
FOOTER = bytearray([0x0d, 0x0a])  # 帧尾


def send_data(Target_x,Target_y):
    global uart;
    FistByte = bytearray([0xb3,0xb3])     # 帧头
    uart.write(FistByte)                  # 写到串口
    uart.write(str(Target_x))
    uart.write(bytearray([0x20]))   # 发送空格
    uart.write(str(Target_y))
    uart.write(bytearray([0x20]))

    EndByte = bytearray([0x0d,0x0a])     # 帧尾,换行和回车的ascll
    uart.write(EndByte)

def find_max(blobs):
    max_size = 0
    for blob in blobs:
        if blob.pixels() > max_size:
            max_blob = blob
            max_size = blob.pixels()
    return max_blob

# Function to calculate PID output
def pid_control(error, last_error, integral, Kp, Ki, Kd):
    # Proportional term
    proportional = Kp * error

    # Integral term
    integral += error
    integral_term = Ki * integral

    # Derivative term
    derivative = Kd * (error - last_error)

    # PID output
    output = proportional + integral_term + derivative

    # Update last error
    last_error = error

    return output, last_error, integral

# Function to send control signal via UART
def output_to_servo(output_x, output_y):
    Servo_dx = int(math.atan2(output_x * 1.8/66 , 15) * 180 / 3.14)
    Servo_dy = int(math.atan2(output_y * 1.8/66 , 15) * 180 / 3.14)
    return Servo_dx + 128,Servo_dy + 128



# Main loop
while(True):
    img = sensor.snapshot()  # Capture an image

    # Detect blobs based on the green threshold
    blobs = img.find_blobs([green_threshold])
    if blobs:
        max_blob = find_max(blobs)  # Find the largest blob

        # Draw a rectangle and crosshair on the largest blob
        img.draw_rectangle(max_blob.rect())
        img.draw_cross(max_blob.cx(), max_blob.cy())

        # Get the center coordinates of the largest blob
        blob_cx = max_blob.cx()
        blob_cy = max_blob.cy()

        # Calculate error (deviation from image center)
        error_x = img_center_x - blob_cx
        error_y = img_center_y - blob_cy

        # PID control for X axis
        output_x, last_error_x, integral_x = pid_control(error_x, last_error_x, integral_x, Kp, Ki, Kd)

        # PID control for Y axis
        output_y, last_error_y, integral_y = pid_control(error_y, last_error_y, integral_y, Kp, Ki, Kd)

        Servo_data[0],Servo_data[1] = output_to_servo(output_x,output_y)
        send_data(Servo_data[0],Servo_data[1])      # 发送数据
        # Print debug information
        print("Error X:", Servo_data[0], "Output X:", output_x)
        print("Error Y:", error_y, "Output Y:", output_y)
    else:

        print('not found!')  # No blobs detected
