import sys
import tempfile

import yaml
from logger import Logger


class PageNode:
    def __init__(self, name):
        self.name = name
        self.children = []
        self.current_index = 0
        self.level = 0  # ! self assigned while print_tree()
        self.log = Logger()

    def add_child(self, child):
        self.children.append(child)

    def print_tree(self, level=0):
        self.level = level
        self.log.info(str(self.level) + ")" + "  " * level + self.name)
        for child in self.children:
            child.print_tree(level + 1)


class PageTree:
    _instance = None
    _initialized = False

    def __new__(cls, ui_eez_filename=None):
        if cls._instance is None:
            if ui_eez_filename is None:
                raise ValueError(
                    "ui_eez_filename is required for the first instantiation of PageTree"
                )
            cls._instance = super().__new__(cls)
        return cls._instance

    def __init__(self, ui_eez_filename=None):
        if hasattr(self, "_initialized") and self._initialized:
            return  # Avoid reinitializing for singleton instance

        if ui_eez_filename is None:
            raise ValueError("ui_eez_filename is required for PageTree initialization.")

        # Initialize all members here
        self.root = PageNode("Root")
        self.log = Logger()
        self._parse_flx_page_tree(ui_eez_filename)
        self.print_tree()  # self-assigned level - don't remove
        self._initialized = True

    def _format_node(self, node, indent=0):
        indent_str = " " * indent
        formatted = (
            f"{indent_str}{{\n"
            f'{indent_str}    page_name: "{node.name}",\n'
            f'{indent_str}    page_display_name: "{node.name}",\n'
            f"{indent_str}    num_sub_pages: {len(node.children)},\n"
        )

        if node.children:
            formatted += f"{indent_str}    sub_pages: [\n"
            child_formatted = [
                self._format_node(child, indent + 6) for child in node.children
            ]
            formatted += ",\n".join(child_formatted) + "\n"
            formatted += f"{indent_str}    ]\n"
        else:
            formatted += f"{indent_str}    sub_pages: []\n"

        formatted += f"{indent_str}}}"
        return formatted

    def print_and_save_tree(self, filename):
        formatted_tree = self._format_node(self.root)
        print(formatted_tree)

        with open(filename, "w") as file:
            file.write(formatted_tree + "\n")

    def get_feature_name(self, index):
        # Check if root and children exist
        if not hasattr(self, "root") or not self.root or not self.root.children:
            print("error: self.root or self.root.children are empty", flush=True)
            return "-"

        main = self.root.children[0]  # First level child (main)

        # Check if 'main' has children and if the index is valid
        if not main.children or len(main.children) <= index < 0:
            print(f"index {index} is out of bounds", flush=True)
            return "-"

        # Return the name of the second-level child
        return main.children[index].name

    def _parse_flx_page_tree(self, ui_eez_filename):
        def save_page_tree_csv(tree, file, level=0):
            line = "," * level + tree.get("page_name", "Unnamed Page") + "\n"
            file.write(line)

            # Recursively write each sub-page if there are any
            sub_pages = tree.get("sub_pages", [])
            for sub_page in sub_pages:
                save_page_tree_csv(sub_page, file, level + 1)

        with open(ui_eez_filename) as f:
            data = yaml.safe_load(f)

        # Extract the tree structure from the specified variable
        tree = ""
        for item in data["variables"]["globalVariables"]:
            if item["name"] == "main_page_info":
                tree = item["defaultValue"]
        assert tree, "Expected tree to be non-empty."

        # Load the page tree structure
        page_tree = yaml.safe_load(tree)

        with tempfile.NamedTemporaryFile(
            mode="w+", suffix=".csv", delete=True
        ) as temp_file:
            for sub_page in page_tree.get("sub_pages", []):
                save_page_tree_csv(sub_page, temp_file, level=0)

            temp_file.seek(0)
            self._parse_csv(temp_file.name)

    def _parse_csv(self, filepath):
        with open(filepath) as file:
            if not file:
                print(f"Error: Unable to open file {filepath}")
                sys.exit(0)

            stack = [self.root]

            for line in file:
                line_strip = line.strip()

                # Count leading commas to determine level
                level = self.count_leading_commas(line_strip)
                page_name = self.remove_ending_commas(line_strip[level:])

                new_node = PageNode(page_name)

                # Adjust the stack so that it contains only nodes up to the correct level
                if len(stack) > level + 1:
                    stack = stack[: level + 1]

                # Add new node as a child of the current parent
                parent_node = stack[-1]
                parent_node.add_child(new_node)

                # Add the new node to the stack
                stack.append(new_node)

        return True

    def print_tree(self):
        self.log.info("-" * 18 + " using Navigation Tree " + "-" * 18)
        self.root.print_tree()
        self.log.info("-" * 18 + " end of Navigation Tree " + "-" * 18 + "\n\n")

    @staticmethod
    def count_leading_commas(line):
        # Count the leading commas which determine the depth level
        return len(line) - len(line.lstrip(","))

    @staticmethod
    def remove_ending_commas(page_name):
        # Remove any trailing commas to clean up the page name
        return page_name.rstrip(",")


def traverse_pagetree_recursive_yaml(
    current_node, output_yaml, root_name, enter_on_right=False
):
    assert current_node

    # Process the current node as "OK" when visiting it initially, except "Main Menu"
    if current_node.name != root_name:
        entry = {
            "Precondition": "none",
            "Command": "navigate Right"
            if enter_on_right
            else "press joystick ok button",
            "Postcondition": current_node.name,
        }
        output_yaml.append(entry)
        print("-", entry)  # Output the YAML entry to stdout

    if not current_node.children:
        # No children - return back
        return

    # Recursively traverse through each child node
    _print_on_right = False
    for child in current_node.children:
        traverse_pagetree_recursive_yaml(child, output_yaml, root_name, _print_on_right)
        _print_on_right = True

    # Traverse left back to the first child
    for index in range(len(current_node.children) - 2, -1, -1):
        left_entry = {
            "Precondition": "none",
            "Command": "navigate Left",
            "Postcondition": current_node.children[index].name,
        }
        output_yaml.append(left_entry)
        print("-", left_entry)  # Output the YAML entry to stdout

    # Now print return to self
    if current_node.name != root_name:
        rear_entry = {
            "Precondition": "none",
            "Command": "press rear button",
            "Postcondition": current_node.name,
        }
        output_yaml.append(rear_entry)
        print("-", rear_entry)  # Output the YAML entry to stdout


def traverse_pagetree_yaml(root, root_name):
    output_yaml = []  # Store all YAML commands here

    if not root or not root.children:
        return

    root = root.children[0]

    # Start the recursive traversal from the root node
    traverse_pagetree_recursive_yaml(root, output_yaml, root_name)

    # Convert the YAML node to a string and trim leading/trailing whitespace
    yaml_str = yaml.dump(output_yaml, default_flow_style=False)

    # Write to a file
    with open("tests/traverse_test.yaml", "w") as outfile:
        outfile.write("---\n")
        outfile.write(yaml_str)
