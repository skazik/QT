import serial
import time
import sys

# Set the correct port for your device (e.g., /dev/ttyUSB0 or /dev/ttyACM0)
SERIAL_PORT = '/dev/ttyACM0'
# SERIAL_PORT = '/dev/ttyUSB0'
BAUD_RATE = 115200
ser = 0

def open_serial():
    # Open the serial port
    global ser
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)

    if ser.is_open:
        print("Serial port opened successfully")
    else:
        print("Failed to open serial. Abort")
        sys.exit()


def close_serial():
    global ser
    # Close the serial port
    ser.close()
    print("Serial port closed")


def send_to_device(data_to_send, timeout):
    global ser
    print("send_to_device:", data_to_send, ":", timeout, flush=True)
    ser.write(data_to_send)
    time.sleep(timeout)
