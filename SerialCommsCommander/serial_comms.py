import serial
import time
import sys


class Timeout:
    def __init__(self, timeout_duration):
        self.timeout_duration = timeout_duration  # Timeout duration in sec
        self.start_time = None

    def set(self, timeout_duration):
        """Sets the timeout and starts the timer."""
        self.timeout_duration = timeout_duration
        self.start_time = time.time()

    def expired(self):
        """Checks if the timeout has expired."""
        if self.start_time is None:
            raise Exception("Timeout not set.")
        return (time.time() - self.start_time) >= self.timeout_duration


ser = 0
def open_serial(serial_port, baud_rate):
    # Open the serial port
    print("Opening comms to " + serial_port + " rate:" + str(baud_rate))

    global ser
    ser = serial.Serial(serial_port, baud_rate, timeout=1)

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


def read_from_device():
    # Read the response from T-Watch
    while ser.in_waiting > 0:
        incoming_data = ser.readline().decode('utf-8').strip()
        print(f"[Received]:: {incoming_data}")


def send_to_device(data_to_send, timeout):
    global ser
    print("send_to_device:", data_to_send, ", timeout:", timeout, flush=True)
    ser.write(data_to_send)

    tm = Timeout(1)
    tm.set(timeout+1)

    while not tm.expired():
        read_from_device()
