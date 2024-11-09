import os
import random
import time
from io import StringIO

import pandas as pd
import yaml
from logger import Logger
from navigator import Navigator
from page_tree import PageTree
from path_finder import PathFinder
from serial_connection import SerialConnection


class Translator:
    _instance = None
    _initialized = False

    def __new__(cls):
        if cls._instance is None:
            cls._instance = super().__new__(cls)
        return cls._instance

    def __init__(self):
        if hasattr(self, "_initialized") and self._initialized:
            return

        self.log = Logger()
        self.serial_connection = SerialConnection()
        self.precondition = None
        self.command = None
        self.postcondition = None
        self.cmd = None
        self.preprocessing = False
        self.skip_root_name = "page_startup"

        self._initialized = True

    def get_serial_connection(self):
        return self.serial_connection

    def get_root_name(self):
        return self.skip_root_name

    def parse_csv_string(self, csv_string, header=None):
        csv_file = StringIO(csv_string)
        df = pd.read_csv(csv_file, header=header, skip_blank_lines=False, dtype=str)
        df = df.fillna("")
        return df.values.tolist()

    def rand_and_send(self, count_max):
        array = ["u", "d", "l", "ri", "k", "re", "re"]
        self.log.info("-" * 20 + "rand_and_send started")

        random.seed(int(time.time()))

        try:
            for _count in range(count_max):
                parsed_data = self.parse_csv_string(random.choice(array))
                for row in parsed_data:
                    cmd_bytearray = self.translate_script_cmd(row[0])
                    self.serial_connection.send_to_device(cmd_bytearray, 1)
        except KeyboardInterrupt:
            self.log.info("\nCtrl+C detected! Exiting the loop safely.")

        self.log.info("-" * 20 + f"rand_and_send completed {count_max} commands")

    def convert_command(self):
        command_lower = self.command.lower()

        if all(word in command_lower for word in ("press", "joystick", "ok")) or all(
            word in command_lower for word in ("press", "joystick", "button")
        ):
            self.cmd = "K"

        elif all(word in command_lower for word in ("press", "rear")) or all(
            word in command_lower for word in ("rear", "button")
        ):
            self.cmd = "rear"

        elif all(
            word in command_lower for word in ("move", "joystick", "x=1.0")
        ) or all(word in command_lower for word in ("right",)):
            self.cmd = "Right"

        elif all(
            word in command_lower for word in ("move", "joystick", "x=-1.0")
        ) or all(word in command_lower for word in ("left",)):
            self.cmd = "Left"

        elif all(word in command_lower for word in ("move", "joystick", "y=1.0")):
            self.cmd = "Up"

        elif all(word in command_lower for word in ("move", "joystick", "y=-1.0")):
            self.cmd = "Down"

        elif all(word in command_lower for word in ("home",)):
            self.cmd = "Home"

        elif all(word in command_lower for word in ("start",)):
            self.cmd = "Start"

        else:
            self.log.warning(f"Unknown command: {self.command}")
            self.cmd = None

        if self.cmd:
            self.log.trace(f"Converted command '{self.command}' to '{self.cmd}'")

    def parse_and_send_entry(self, entry):
        self.precondition = None
        self.command = None
        self.postcondition = None

        for key, value in entry.items():
            key_lower = key.lower()
            if key_lower == "precondition":
                self.precondition = value
            elif key_lower == "command":
                self.command = value.lower()
            elif key_lower == "postcondition":
                self.postcondition = value
            else:
                self.log.warning(f"Unexpected key: {key}")

        self.log.debug(
            f"Parsed Entry:\n Precondition: {self.precondition},\n"
            f" Command: {self.command},\n Postcondition: {self.postcondition}"
        )

        if self.precondition is not None and self.command == "start":
            current_name = Navigator().get_current_view_name()
            if current_name != self.postcondition:
                self.log.critical(
                    f'in preprocess: current "{current_name}" mismatch postcondition '
                    f'"{self.postcondition}"'
                )
                assert current_name == self.postcondition
            return

        if self.precondition is not None and self.precondition.lower() != "none":
            current_name = Navigator().get_current_view_name()
            if current_name == "none":
                self.log.warning(f'navigator current "{current_name}" ?')
            elif current_name != self.precondition:
                if self.preprocessing:
                    self.log.critical(
                        f'in preprocess: current "{current_name}" mismatch precondition '
                        f'"{self.precondition}"'
                    )
                    assert current_name == self.precondition

                self.log.debug(
                    f'current "{current_name}" mismatch precondition "{self.precondition}"'
                )
                output_yaml = PathFinder().traverse_pagetree_path(
                    PageTree().root,
                    self.skip_root_name,
                    current_name,
                    self.precondition,
                )
                save_cmd = self.command
                self.preprocessing = True
                self.log.info(
                    "-" * 10 + f'navigating to "{self.precondition}" view ---'
                )
                for _entry in output_yaml:
                    self.parse_and_send_entry(_entry)

                self.log.info("-" * 10 + "completed precondition auto processing ---\n")
                self.command = save_cmd
                self.preprocessing = False

        self.convert_command()  # result: self.cmd is filled
        cmd_bytearray = self.translate_script_cmd(self.cmd)
        self.serial_connection.send_to_device(cmd_bytearray, 1)

    def is_valid_yaml(self, file_path):
        if os.path.splitext(file_path)[1].lower() not in [".yaml", ".yml"]:
            return False

        try:
            with open(file_path) as file:
                yaml.safe_load(file)
            return True  # The file is a valid YAML file
        except yaml.YAMLError as e:
            self.log.error(f"YAML Error: {e}")
            return False

    def translate_and_send_from_yaml(self, file_path):
        try:
            with open(file_path) as file:
                yaml_content = yaml.safe_load(file)

        except yaml.YAMLError as e:
            self.log.error(f"YAML Error: {e}")

        if isinstance(yaml_content, dict):
            yaml_content = [yaml_content]

        try:
            for entry in yaml_content:
                if isinstance(entry, dict):
                    self.parse_and_send_entry(entry)
                else:
                    self.log.warning(f"  Non-dictionary entry: {entry}")

        except KeyboardInterrupt:
            self.log.info("\nCtrl+C detected! Exiting the loop safely.")

    def translate_and_send_plain(self, file_path):
        with open(file_path) as file:
            for index, line in enumerate(file):
                self.log.debug(line.strip())  # for script debug
                if index == 0:
                    self.log.debug("pylint : no input?")
                    # input("sync first page and press ENTER to continue...")

                if line.startswith(("//", "#")):
                    self.log.info(line.strip())
                    continue

                parsed_data = self.parse_csv_string(line)
                for row in parsed_data:
                    cmd_bytearray = self.translate_script_cmd(row[0])
                    timeout_int = int(row[1]) if len(row) > 1 else 1
                    self.serial_connection.send_to_device(cmd_bytearray, timeout_int)

    def translate_and_send(self, file_path):
        if not os.path.isfile(file_path):
            self.log.error(f"File '{file_path}' does not exist.")
            return False

        if self.is_valid_yaml(file_path):
            self.translate_and_send_from_yaml(file_path)
        else:
            self.translate_and_send_plain(file_path)

        return True

    def translate_script_cmd(self, input_str):
        input_str = input_str.lower()

        def translate_key_to_cmd(key):
            data_to_send = bytearray(
                [
                    0x42,
                    0x67,
                    0x6E,  # 'Bgn'
                    0x0E,  # HandleControllerSerialCommand = 14
                    0x6D,  # CRC8
                    0x2A,  # '42' cmd type
                    0x72,  # 0x72 'r'
                    0x2D,
                    0x2D,
                    0x2D,
                    0x2D,
                    0x2D,
                    0x2D,
                    0x00,
                    0x00,
                ]
            )  # CRC16

            key_mapping = {
                "Key_Up": "U",
                "Key_Down": "D",
                "Key_Left": "L",
                "Key_Right": "R",
                "Key_OK": "K",
                "Key_Rear": "r",
                "Key_None": "\0",
            }
            data_to_send[6] = ord(key_mapping.get(key, ""))
            return data_to_send

        navigator = Navigator()
        if input_str in {"u", "up"}:
            navigator.on_up()
            return translate_key_to_cmd("Key_Up")
        if input_str in {"d", "down", "dn"}:
            navigator.on_down()
            return translate_key_to_cmd("Key_Down")
        if input_str in {"l", "left", "lt"}:
            navigator.on_left()
            return translate_key_to_cmd("Key_Left")
        if input_str in {"ri", "right", "rt"}:
            navigator.on_right()
            return translate_key_to_cmd("Key_Right")
        if input_str in {"k", "ok"}:
            navigator.on_enter()
            return translate_key_to_cmd("Key_OK")
        if input_str in {"re", "rb", "rear", "rbutton", "rearbutton"}:
            navigator.on_back()
            return translate_key_to_cmd("Key_Rear")

        return translate_key_to_cmd("Key_None")
