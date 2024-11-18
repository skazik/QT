#!/usr/bin/python3
import argparse
import os

from dev_tests import test_and_validate
from logger import Logger
from page_tree import PageTree
from translations import Translator

SERIAL_PORT = "/dev/ttyACM0"
BAUD_RATE = 115200
RAND_DEFAULT = 0
INTERVAL = 1


def main():
    parser = argparse.ArgumentParser(
        description="Read `CMD` <command script>.yaml and send to device.",
        usage="%(prog)s [-h] -c CMD -u UI [-p PORT] [-b BAUD] [-r RAND] [-i INTERVAL] [-t] [-d] [-g]",
    )
    parser.add_argument(
        "-c",
        "--cmd",
        type=str,
        required=True,
        help="(Required) Path to the yaml command script file (e.g., 'tests/test_script.yaml')",
    )
    parser.add_argument(
        "-u",
        "--ui",
        type=str,
        required=True,
        help="(Required) Path to the UI file (e.g., '<path_to>ui.eez-project')",
    )
    parser.add_argument(
        "-p",
        "--port",
        type=str,
        default=SERIAL_PORT,
        help='Optional SERIAL_PORT, default: "/dev/ttyACM0"',
    )
    parser.add_argument(
        "-b",
        "--baud",
        type=int,
        default=BAUD_RATE,
        help="Optional BAUD_RATE, default: 115200.",
    )
    parser.add_argument(
        "-r",
        "--rand",
        type=int,
        default=RAND_DEFAULT,
        help="Optional random commands, default count: 0 (none).",
    )
    parser.add_argument(
        "-i",
        "--interval",
        type=int,
        default=INTERVAL,
        help="Optional interval between sending CMD to device, default 1 second.",
    )
    parser.add_argument("-t", "--test", action="store_true", help="Run in test mode.")
    parser.add_argument("-d", "--debug", action="store_true", help="Run in debug mode.")
    parser.add_argument(
        "-g",
        "--generate",
        action="store_true",
        help="Generate tests/traverse_test.yaml",
    )
    parser.add_argument("-f", "--find", action="store_true", help="Test path_finder.")

    args = parser.parse_args()

    if args.debug:
        log = Logger()
        log.set_debug_mode()

    translator = Translator()
    translator.set_interval(args.interval)
    serial = translator.get_serial_connection()

    if args.test:
        print("Test mode is enabled. ! Serial comms is not used !")
        serial.set_test_mode()

    serial.open_serial(args.port, args.baud)

    # initiate PageTree Singleton here with path to ui.eez-proheject once!
    ptree = PageTree(args.ui)
    test_and_validate(ptree, args.test, args.generate, args.find)

    if args.rand > RAND_DEFAULT:
        translator.rand_and_send(args.rand)
    elif os.path.isfile(args.cmd):
        translator.translate_and_send(args.cmd)
    else:
        print(f"Error: The file '{args.cmd}' does not exist.", flush=True)

    serial.close_serial()


if __name__ == "__main__":
    main()
