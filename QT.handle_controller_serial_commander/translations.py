import random
import time
from io import StringIO

import pandas as pd
from navigator import Navigator
from serial_comms import SerialConnection

serial_connection = SerialConnection()


def get_serial_connection():
    return serial_connection


import json
import os

def csv_to_json(filename):
    json_output = []

    with open(filename) as file:
        for line in file:
            line = line.strip()

            # Skip comments and empty lines
            if line.startswith("//") or not line:
                continue

            # Parse the line into command, timeout, and expected values
            parts = line.split(',')
            command = parts[0]
            timeout = parts[1] if len(parts) > 1 else "1"
            expected = parts[2] if len(parts) > 2 else "none"

            # Append to JSON output list
            json_output.append({
                "command": command,
                "timeout": timeout,
                "expected": expected
            })

    # Convert the list to JSON format
    json_data = json.dumps(json_output, indent=4)

    # Extract the part after the first period
    base_name = filename.split('.', 1)[-1]
    # Change the file extension to .json
    json_filename = os.path.splitext(base_name)[0] + '.json'

    # Write the JSON output to the file
    with open(json_filename, 'w') as json_file:
        json_file.write(json_data)

    return json_filename


def parse_csv_string(csv_string, header=None):
    """
    Parses a CSV string and returns the data as a list of rows,
    where each row is a list of values.
    """
    csv_file = StringIO(csv_string)
    df = pd.read_csv(csv_file, header=header, skip_blank_lines=False, dtype=str)
    df = df.fillna("")
    return df.values.tolist()


def rand_and_send(count_max, test):
    array = ["u", "d", "l", "ri", "k", "re", "re"]

    # Seed the random number generator with the current time in seconds
    random.seed(int(time.time()))
    if not test:
        input("sync start <Bend & Rotate> and press ENTER to continue...")

    try:
        for _count in range(count_max):
            parsed_data = parse_csv_string(random.choice(array))
            for row in parsed_data:
                cmd_bytearray = translate_script_cmd(row[0])
                if not test:
                    serial_connection.send_to_device(cmd_bytearray, 1)
    except KeyboardInterrupt:
        print("\nCtrl+C detected! Exiting the loop safely.", flush=True)
    except Exception as e:
        print(f"An error occurred: {e}", flush=True)


def translate_and_send(file_path):
    try:
        with open(file_path) as file:
            for index, line in enumerate(file):
                # print(line.strip(), flush=True)  # for script debug

                if line.startswith("//") or line.startswith("#"):
                    print(line.strip(), flush=True)
                    if index == 0:
                        input(
                            "sync Handle Controller starting screen with script and press ENTER to continue..."
                        )
                    continue
                parsed_data = parse_csv_string(line)
                for row in parsed_data:
                    cmd_bytearray = translate_script_cmd(row[0])
                    timeout_int = int(row[1]) if len(row) > 1 else 1
                    serial_connection.send_to_device(cmd_bytearray, timeout_int)
    except KeyboardInterrupt:
        print("\nCtrl+C detected! Exiting the loop safely.", flush=True)
    except FileNotFoundError:
        print(f"The file at {file_path} was not found.", flush=True)
    except Exception as e:
        print(f"An error occurred: {e}", flush=True)


def translate_script_cmd(input_str):
    # Convert input to lowercase
    input_str = input_str.lower()

    # Define a simple hash function (can use Python's hash if needed)
    def hash_string(s):
        return hash(s)

    # Simulated 'translate_key_to_cmd' function to return string
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
        }
        #        print(key_mapping.get(key, ''), flush=True)
        data_to_send[6] = ord(key_mapping.get(key, ""))
        return data_to_send

    navigator = Navigator()
    # Calculate the hash for comparison
    if input_str in ["u", "up"]:
        return translate_key_to_cmd("Key_Up")
    elif input_str in ["d", "down", "dn"]:
        return translate_key_to_cmd("Key_Down")
    elif input_str in ["l", "left", "lt"]:
        navigator.on_left()
        return translate_key_to_cmd("Key_Left")
    elif input_str in ["ri", "right", "rt"]:
        navigator.on_right()
        return translate_key_to_cmd("Key_Right")
    elif input_str in ["k", "ok"]:
        navigator.on_enter()
        return translate_key_to_cmd("Key_OK")
    elif input_str in ["re", "rb", "rear", "rbutton", "rearbutton"]:
        navigator.on_back()
        return translate_key_to_cmd("Key_Rear")
    else:
        return ""
