import argparse
from collections import deque
import os

from page_tree import PageTree
from translations import Translator

SERIAL_PORT = "/dev/ttyACM0"  # SERIAL_PORT = '/dev/ttyUSB0'  #
BAUD_RATE = 115200
RAND_DEFAULT = 0
SCRIPT = "test_script.yaml"


def find_shortest_path(root, start_name, end_name):
    """
    Finds the path between two nodes in a PageTree structure by sequentially searching for start and end.

    Args:
        root (PageNode): The root node of the tree.
        start_name (str): The name of the starting node.
        end_name (str): The name of the target node.

    Returns:
        list: The path from start_name to end_name as a list of node names.
              Returns an empty list if either node is not found.
    """
    # This will store the sequence of node names between start and end
    path = []
    found_first = None  # Keeps track of which node was found first
    found_path = False  # Becomes True when both nodes have been found

    # Helper function to traverse the tree and find the path
    def traverse(node):
        nonlocal found_first, found_path

        if found_path:
            return  # Stop traversal once the path is found

        # Add the current node to the path
        path.append(node.name)

        # Check if the current node matches the start or end
        if node.name == start_name or node.name == end_name:
            if not found_first:
                # Found the first node (either start or end)
                found_first = node.name
            else:
                # Found the second node (either end or start)
                found_path = True
                return

        # Continue searching through children if both nodes aren't found yet
        for child in node.children:
            traverse(child)
            if found_path:
                return  # Stop searching if the path is already found

        # If we are backtracking and haven't found both nodes yet, remove the node from the path
        if not found_path:
            path.pop()

    # Start traversing the tree from the root
    traverse(root)

    # If the path is found but the last node was the start_name, reverse the path
    if found_first == start_name and found_path:
        return path[::-1]
    elif found_first == end_name and found_path:
        return path
    else:
        # If we didn't find both nodes, return an empty list
        return []

# Example usage:
# Assuming `tree` is an instance of PageTree and `tree.root` is the root PageNode.
# start_node_name = "Calibrate Joystick Selected"
# end_node_name = "Joint Control Segment"
# path = find_shortest_path(tree.root, start_node_name, end_node_name)
# print("Shortest path from", start_node_name, "to", end_node_name, ":", path)

def test_and_validate(ptree, translator):
    # for testing & validation - not used,
    # please don't delete used to supress ruff error of unused vars

    start_node_name = "Calibrate Joystick Selected"
    end_node_name = "Joint Control Segment"
    path = find_shortest_path(ptree.root, start_node_name, end_node_name)
    print("=" * 42, flush=True)
    print("Shortest path from", start_node_name, "to", end_node_name, ":", path)
    print("=" * 42, flush=True)

    result = ptree.verify_unique_hashes(ptree.root)
    print("All hash values are unique:", result)
    return

    ptree.print_tree()
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
        help='optional, default: "test_script.yaml"',
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
    parser.add_argument("-t", "--test", action="store_true", help="Run in test mode.")
    parser.add_argument("-d", "--debug", action="store_true", help="Run in debug mode.")

    # Parse command-line arguments
    args = parser.parse_args()

    if args.debug:
        from logger import Logger

        log = Logger()
        log.set_debug_mode()

    translator = Translator()
    serial = translator.get_serial_connection()

    if args.test:
        print("Test mode is enabled. ! Serial comms is not used !")
        serial.set_test_mode()

    serial.open_serial(args.port, args.baud)

    # initiate PageTree Singleton here and call dummy to avoid ruff error
    ptree = PageTree()
    test_and_validate(ptree)

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
