import re
import sys
import time

import serial
from navigator import Navigator
from page_tree import PageTree


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


class SerialConnection:
    def __init__(self):
        self.ser = None

    def open_serial(self, serial_port, baud_rate):
        # Open the serial port
        print(
            "Opening comms to " + serial_port + " rate: " + str(baud_rate), flush=True
        )
        try:
            self.ser = serial.Serial(serial_port, baud_rate, timeout=1)

            if self.ser.is_open:
                print("Serial port opened successfully", flush=True)
            else:
                print("Failed to open serial. Abort", flush=True)
                sys.exit()
        except serial.SerialException as e:
            print(f"Error opening serial port: {e}", flush=True)
            sys.exit()

    def close_serial(self):
        # Close the serial port
        if self.ser and self.ser.is_open:
            self.ser.close()
            print("Serial port closed", flush=True)
        else:
            print("Serial port is not open", flush=True)

    def read_from_device(self):
        # Read the response from the device
        if self.ser and self.ser.is_open:
            navigator = Navigator()
            while self.ser.in_waiting > 0:
                incoming_data = self.ser.readline().decode("utf-8").strip()
                if (
                    "are_devices_connected.hpp" in incoming_data
                    or "handle controller loop" in incoming_data
                    or "Base station - rssi" in incoming_data
                ):
                    continue
                elif "view_idx:" in incoming_data:
                    match = re.search(r"view_idx:(\d)", incoming_data)
                    if match:
                        current = navigator.get_current_view_idx()
                        reported = int(match.group(1))
                        if reported == current:
                            view = navigator.get_current_view_name()
                            print(f"        [in sync]:{view}", flush=True)
                        else:
                            print(
                                f"    [error: view mismatch]------------------ "
                                f"{current} != {reported}",
                                flush=True,
                            )

                elif "feature_idx:" in incoming_data:
                    match = re.search(r"feature_idx:(\d)", incoming_data)
                    if match:
                        page_tree = PageTree()
                        reported = page_tree.get_feature_name(int(match.group(1)))
                        if navigator.get_current_feature() == reported:
                            print(f"        [in sync]:{reported}", flush=True)
                        else:
                            print(
                                f"    [error: feature mismatch]--------------- "
                                f"{navigator.get_current_feature()} != {reported}",
                                flush=True,
                            )
                # for debugging unexpected delivery from device
                # else:
                #     print(f"[Received]:: {incoming_data}", flush=True)
        else:
            print("Serial port is not open", flush=True)

    def send_to_device(self, data_to_send, timeout):
        if self.ser and self.ser.is_open:
            self.ser.write(data_to_send)
            tm = Timeout(1)
            tm.set(timeout)
            while not tm.expired():
                self.read_from_device()
        else:
            print("Serial port is not open", flush=True)
