import argparse
import os
import sys

sys.path.append("src")

from logger import Logger
from page_tree import PageTree, traverse_pagetree_yaml
from path_finder import PathFinder
from translations import Translator

SERIAL_PORT = "/dev/ttyACM0"  # SERIAL_PORT = '/dev/ttyUSB0'  #
BAUD_RATE = 115200
RAND_DEFAULT = 0
SCRIPT = "tests/test_script.yaml"
SKIP_ROOT_NAME = "Main Menu"
SLOW = 0


def test_and_validate(log, ptree, generate, test):
    # for testing & generation of test files
    # please don't delete, used to supress ruff error of unused ptree
    if generate:
        traverse_pagetree_yaml(ptree.root, SKIP_ROOT_NAME)

    return

    if test:
        path_finder_test = PathFinder()
        start_name = "Elbow 1 Selected"
        end_name = "Reset Position"

        for _i in range(2):
            output_yaml = path_finder_test.traverse_pagetree_path(
                ptree.root, SKIP_ROOT_NAME, start_name, end_name
            )
            for entry in output_yaml:
                if "Command" in entry:
                    log.debug(f"- Command: {entry['Command']}")
                if "Postcondition" in entry:
                    log.debug(f"  Postcondition: {entry['Postcondition']}")
            log.debug(f"--- path from {start_name} to {end_name} done ---\n")
            # Swap start_name and end_name for the next iteration
            start_name, end_name = end_name, start_name

    ptree.print_and_save_tree("tmp._ui.extract.out.yaml")


def main():
    # Set up argument parser
    parser = argparse.ArgumentParser(
        description="Read `SCRIPT` script and send to device."
    )
    # Adding an optional argument
    parser.add_argument(
        "-s",
        "--script",
        type=str,
        default=SCRIPT,
        help='optional, default: "tests/test_script.yaml"',
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
        help="optional random commands, default count: 0 (none).",
    )
    parser.add_argument(
        "-l",
        "--slow",
        type=int,
        default=SLOW,
        help="optional slow down command send by seconds, default: 0.",
    )
    parser.add_argument("-t", "--test", action="store_true", help="run in test mode.")
    parser.add_argument("-d", "--debug", action="store_true", help="run in debug mode.")
    parser.add_argument(
        "-g",
        "--generate",
        action="store_true",
        help="generate tests/traverse_test.yaml",
    )

    # Parse command-line arguments
    args = parser.parse_args()

    log = Logger()
    if args.debug:
        log.set_debug_mode()

    translator = Translator()
    serial = translator.get_serial_connection()
    serial.set_slow_down(args.slow)

    if args.test:
        print("Test mode is enabled. ! Serial comms is not used !")
        serial.set_test_mode()

    serial.open_serial(args.port, args.baud)

    # initiate PageTree Singleton here and call for tests and generates
    ptree = PageTree()
    test_and_validate(log, ptree, args.generate, args.test)

    if args.rand > RAND_DEFAULT:
        translator.rand_and_send(args.rand, args.test)
    # Validate if file exists
    elif os.path.isfile(args.script):
        translator.translate_and_send(args.script)
    else:
        print(f"Error: The file '{args.script}' does not exist.", flush=True)

    serial.close_serial()


if __name__ == "__main__":
    main()
