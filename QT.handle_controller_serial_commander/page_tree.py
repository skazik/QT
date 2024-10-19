import os
import sys

import yaml


class PageNode:
    def __init__(self, name):
        self.name = name
        self.children = []
        self.current_index = 0  # Add this attribute to track the current child index

    def add_child(self, child):
        self.children.append(child)

    def print_tree(self, level=0):
        print("  " * level + self.name, flush=True)
        for child in self.children:
            child.print_tree(level + 1)


class PageTree:
    _instance = None

    def __new__(cls):
        if cls._instance is None:
            cls._instance = super().__new__(cls)
            cls._instance.root = PageNode("Root")
            cls._instance._parse_flx_page_tree()
        return cls._instance

    def __init__(self):
        pass

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
        if not main.children or not (0 <= index < len(main.children)):
            print(f"index {index} is out of bounds", flush=True)
            return "-"

        # Return the name of the second-level child
        return main.children[index].name

    def _parse_flx_page_tree(self):
        def save_page_tree_csv(tree, file, level=0):
            line = "," * level + tree.get("page_display_name", "Unnamed Page") + "\n"
            file.write(line)

            # Recursively write each sub-page if there are any
            sub_pages = tree.get("sub_pages", [])
            for sub_page in sub_pages:
                save_page_tree_csv(sub_page, file, level + 1)

        with open("ui.eez-project") as f:
            data = yaml.safe_load(f)

        # Extract the tree structure from the specified variable
        for item in data["variables"]["globalVariables"]:
            if item["name"] == "main_page_info":
                tree = item["defaultValue"]

        # Load the page tree structure
        page_tree = yaml.safe_load(tree)

        with open("tmp.csv", "w") as output_file:
            for sub_page in page_tree.get("sub_pages", []):
                save_page_tree_csv(sub_page, output_file, level=0)

        self._parse_csv("tmp.csv")
        os.remove("tmp.csv")

    def _parse_csv(self, filepath):
        with open(filepath) as file:
            if not file:
                print(f"Error: Unable to open file {filepath}")
                sys.exit(0)

            stack = [self.root]

            for line in file:
                line = line.strip()

                # Count leading commas to determine level
                level = self.count_leading_commas(line)
                page_name = self.remove_ending_commas(line[level:])

                # Create a new node for the current line
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
        print("------------------ using Navigation Tree ------------------------")
        self.root.print_tree()
        print("----------------- end of Navigation Tree ------------------------\n\n")

    @staticmethod
    def count_leading_commas(line):
        # Count the leading commas which determine the depth level
        return len(line) - len(line.lstrip(","))

    @staticmethod
    def remove_ending_commas(page_name):
        # Remove any trailing commas to clean up the page name
        return page_name.rstrip(",")
