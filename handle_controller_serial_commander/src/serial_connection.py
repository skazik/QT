import re
import sys
import time

import serial
from logger import Logger
from navigator import Navigator
from page_tree import PageTree


class Timeout:
    def __init__(self, timeout_duration):
        self.timeout_duration = timeout_duration  # Timeout duration in sec
        self.start_time = None

    def set(self, timeout_duration):
        self.timeout_duration = timeout_duration
        self.start_time = time.time()

    def expired(self):
        if self.start_time is None:
            raise ValueError("Timeout not set.")
        return (time.time() - self.start_time) >= self.timeout_duration


class SerialConnection:
    def __init__(self):
        self.ser = None
        self.test_mode = False
        self.log = Logger()

    def set_test_mode(self):
        self.test_mode = True

    def open_serial(self, serial_port, baud_rate):
        if self.test_mode:
            return
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
            print("use option --test for testing without serial port", flush=True)
            sys.exit()

    def close_serial(self):
        if self.test_mode:
            return
        if self.ser and self.ser.is_open:
            self.ser.close()
            print("Serial port closed", flush=True)
        else:
            print("Serial port is not open", flush=True)

    def send_to_device(self, data_to_send, timeout):
        if self.test_mode:
            return
        if self.ser and self.ser.is_open:
            self.ser.write(data_to_send)
            tm = Timeout(1)
            tm.set(timeout)
            while not tm.expired():
                self.read_from_device()
        else:
            print("Serial port is not open", flush=True)

    def read_from_device(self):
        if self.test_mode:
            return

        if not (self.ser and self.ser.is_open):
            print("Serial port is not open", flush=True)
            return

        navigator = Navigator()

        while self.ser.in_waiting > 0:
            incoming_data = self.ser.readline().decode("utf-8").strip()

            if self._is_ignored_message(incoming_data):
                continue

            if "VIEW:" in incoming_data:
                self.log.debug(f"[received:]{incoming_data}\n")
                self._check_view_sync(incoming_data, navigator)

            elif "view_idx:" in incoming_data:
                self._check_view_idx_sync(incoming_data, navigator)

            elif "feature_idx:" in incoming_data:
                self._check_feature_sync(incoming_data, navigator)

    def _is_ignored_message(self, incoming_data):
        ignored_patterns = [
            "are_devices_connected.hpp",
            "handle controller loop",
            "Base station - rssi",
        ]
        return any(pattern in incoming_data for pattern in ignored_patterns)

    def _check_view_sync(self, incoming_data, navigator):
        match = re.search(r"VIEW:(.*)", incoming_data)
        if match:
            reported = match.group(1).strip()
            view = navigator.get_current_view_name()
            if reported == view:
                self.log.info(f"[view in sync]:{view}\n")
            else:
                self.log.error(
                    f"    [error: view mismatch]------------------ "
                    f"{reported} != {view}\n"
                )

    def _check_view_idx_sync(self, incoming_data, navigator):
        match = re.search(r"view_idx:(\d)", incoming_data)
        if match:
            current = navigator.get_current_view_idx()
            reported = int(match.group(1))
            if reported == current:
                view = navigator.get_current_view_name()
                print(f"[view in sync]:{view}", flush=True)
            else:
                print(
                    f"    [error: view mismatch]------------------ "
                    f"{current} != {reported}",
                    flush=True,
                )

    def _check_feature_sync(self, incoming_data, navigator):
        match = re.search(r"feature_idx:(\d)", incoming_data)
        if match:
            page_tree = PageTree()
            reported = page_tree.get_feature_name(int(match.group(1)))
            if navigator.get_current_feature() == reported:
                self.log.info(f'[selected feature "{reported}" in sync]\n')
            else:
                self.log.error(
                    f"    [error: selected feature mismatch]--------------- "
                    f"{navigator.get_current_feature()} != {reported}\n"
                )
