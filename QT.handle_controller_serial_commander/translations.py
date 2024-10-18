import csv
import random
import time
from io import StringIO

from navigator import navigator
from serial_comms import SerialConnection

serial_connection = SerialConnection()


def get_serial_connection():
    return serial_connection


def parse_csv_string(csv_string):
    """
    Parses a CSV string and returns the data as a list of rows.

    Args:
        csv_string (str): The CSV data as a string.

    Returns:
        list: A list of rows, where each row is a list of values.
    """
    csv_data = []

    # Use StringIO to treat the string as a file-like object
    csv_file = StringIO(csv_string)

    # Use the csv.reader to parse the CSV string
    csv_reader = csv.reader(csv_file)

    # Iterate through the csv_reader and append rows to the csv_data list
    for row in csv_reader:
        csv_data.append(row)

    return csv_data


def rand_and_send(count_max):
    array = ["u", "d", "l", "ri", "k", "re", "re"]

    # Seed the random number generator with the current time in seconds
    random.seed(int(time.time()))
    input("sync start <Bend & Rotate> and press ENTER to continue...")

    try:
        for _count in range(count_max):
            parsed_data = parse_csv_string(random.choice(array))
            for row in parsed_data:
                cmd_bytearray = translate_script_cmd(row[0])
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
