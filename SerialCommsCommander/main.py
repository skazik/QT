# This Python file uses the following encoding: utf-8
import argparse
import os
import csv
import time
from io import StringIO
from translations import translate_script_cmd
from serial_comms import open_serial, close_serial, send_to_device, read_from_device

# Set the correct port for your device (e.g., /dev/ttyUSB0 or /dev/ttyACM0)
SERIAL_PORT = '/dev/ttyACM0'    # SERIAL_PORT = '/dev/ttyUSB0'
BAUD_RATE = 115200


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


def read_file_line_by_line(file_path):
    try:
        with open(file_path, 'r') as file:
            for line in file:
                print(line.strip())
                parsed_data = parse_csv_string(line)
                for row in parsed_data:
                    # print(row)
                    cmd_bytearray = translate_script_cmd(row[0])
                    timeout_int = int(row[1]) if len(row) > 1 else 1
#                    print("CMD:", row[0], ", timeout:", timeout_int, "->",
#                          cmd_bytearray, flush=True)
                    send_to_device(cmd_bytearray, timeout_int)
    except KeyboardInterrupt:
        print("\nCtrl+C detected! Exiting the loop safely.")
    except FileNotFoundError:
        print(f"The file at {file_path} was not found.")
    except Exception as e:
        print(f"An error occurred: {e}")


def main():
    # Set up argument parser
    parser = argparse.ArgumentParser(description="Read CMD script "
                                     "and send to device.")
    parser.add_argument('filename', type=str,
                        help='The path to the script file')
    # Adding an optional argument
    parser.add_argument('-p', '--port', type=str, default=SERIAL_PORT,
                        help='optional SERIAL_PORT, default: "/dev/ttyACM0"')
    parser.add_argument('-b', '--baud', type=int, default=BAUD_RATE,
                        help='optional BAUD_RATE, default: 115200.')

    # Parse command-line arguments
    args = parser.parse_args()

    # Validate if file exists
    if os.path.isfile(args.filename):
        open_serial(args.port, args.baud)
        read_file_line_by_line(args.filename)
        close_serial()
    else:
        print(f"Error: The file '{args.filename}' does not exist.")


if __name__ == "__main__":
    main()
