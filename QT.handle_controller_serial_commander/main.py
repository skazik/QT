import argparse
import os

from page_tree import PageTree
from translations import csv_to_json, get_serial_connection, rand_and_send, translate_and_send

SERIAL_PORT = "/dev/ttyACM0"  # SERIAL_PORT = '/dev/ttyUSB0'  #
BAUD_RATE = 115200
RAND_DEFAULT = 0
TEST_SCRIPT = "test_script_file.txt"


def test_and_validate():
    # for testing & validation - not used, please don't delete
    ptree = PageTree()
    ptree.print_tree()
    ptree.print_and_save_tree("tmp.ui.eez.def")
    json_file_path = csv_to_json('tmp.test_script_file.txt')
    print(f"JSON data written to: {json_file_path}")


def main():
    # Set up argument parser
    parser = argparse.ArgumentParser(
        description="Read CMD script " "and send to device."
    )
    # Adding an optional argument
    parser.add_argument(
        "-s",
        "--script",
        type=str,
        default=TEST_SCRIPT,
        help='optional, default: "test_script_file.txt"',
    )
    parser.add_argument(
        "-p",
        "--port",
        type=str,
        default=SERIAL_PORT,
        help='optional SERIAL_PORT, default: "/dev/ttyACM0"',
    )
    parser.add_argument(
        "-b",
        "--baud",
        type=int,
        default=BAUD_RATE,
        help="optional BAUD_RATE, default: 115200.",
    )
    parser.add_argument(
        "-r",
        "--rand",
        type=int,
        default=RAND_DEFAULT,
        help="optional random commands, default: 0 (none).",
    )
    parser.add_argument("-t", "--test", action="store_true", help="Run in test mode.")

    # Parse command-line arguments
    args = parser.parse_args()

    if args.test:
        print("Test mode is enabled. Serial comms is not used")
        test_and_validate()
    else:
        # open serail
        get_serial_connection().open_serial(args.port, args.baud)

    if args.rand > RAND_DEFAULT:
        rand_and_send(args.rand, args.test)

    # Validate if file exists
    elif os.path.isfile(args.script):
        translate_and_send(args.script)
    else:
        print(f"Error: The file '{args.script}' does not exist.", flush=True)

    if not args.test:
        # close serial
        get_serial_connection().close_serial()


if __name__ == "__main__":
    main()
