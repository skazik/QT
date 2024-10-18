class PageNode:
    def __init__(self, name):
        self.name = name
        self.children = []
        self.current_index = 0  # Add this attribute to track the current child index

    def add_child(self, child):
        self.children.append(child)

    def print_tree(self, level=0):
        print("  " * level + self.name)
        for child in self.children:
            child.print_tree(level + 1)


class PageTree:
    def __init__(self):
        self.root = PageNode("Root")

    def _format_node(self, node, indent=0):
        indent_str = " " * indent
        formatted = (
            f'{indent_str}{{\n'
            f'{indent_str}    page_name: "{node.name}",\n'
            f'{indent_str}    page_display_name: "{node.name}",\n'
            f'{indent_str}    num_sub_pages: {len(node.children)},\n'
        )

        if node.children:
            formatted += f'{indent_str}    sub_pages: [\n'
            child_formatted = [
                self._format_node(child, indent + 6) for child in node.children
            ]
            formatted += ",\n".join(child_formatted) + "\n"
            formatted += f'{indent_str}    ]\n'
        else:
            formatted += f'{indent_str}    sub_pages: []\n'

        formatted += f'{indent_str}}}'
        return formatted

    def print_and_save_tree(self, filename):
        formatted_tree = self._format_node(self.root)
        print(formatted_tree)

        with open(filename, "w") as file:
            file.write(formatted_tree + "\n")

    def get_feature_name(self, index):
        # Check if root and children exist
        if hasattr(self, "root") and self.root and self.root.children:
            main = self.root.children[0]  # First level child (main)

            # Check if 'main' has children and if the index is valid
            if main.children and 0 <= index < len(main.children):
                return main.children[
                    index
                ].name  # Return the name of the second-level child
            else:
                print(f"index {index} is out of bounds")
        else:
            print("error: self.root or self.root.children are empty")
        return "-"

    def parse_csv(self, filepath):
        try:
            with open(filepath) as file:
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
        except FileNotFoundError:
            print(f"Error: Unable to open file {filepath}")
            return False

    def print_tree(self):
        print("--------------------- Navigation Tree --------------------------")
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


page_tree_instance = PageTree()
page_tree_instance.parse_csv("tabview-tree.csv")
# page_tree_instance.print_tree()
# bpage_tree_instance.print_and_save_tree("tmp")
