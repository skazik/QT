import re

from logger import Logger
from page_tree import PageTree


class Navigator:
    _instance = None

    def __new__(cls):
        if cls._instance is None:
            cls._instance = super().__new__(cls)
        return cls._instance

    def __init__(self):
        if not hasattr(self, "_initialized"):  # To ensure single initialization
            self.history = []
            self.current_level = 0
            self.qdebug_on = True
            self.current_feature = ""
            self.log = Logger()

            # Initialize the Navigator with the root of the PageTree
            page_tree = PageTree()
            self.current_node = page_tree.root if hasattr(page_tree, "root") else None

            # Advance to level 3
            self.on_enter(False)
            self.on_enter(False)

            self._initialized = True  # Mark as initialized

    def get_current_feature(self):
        return self.current_feature

    def get_current_view_idx(self):
        parent = self.history[-1]
        if parent:
            return parent.current_index
        return -1

    def get_current_view_name(self):
        parent = self.history[-1]
        if parent:
            return parent.children[parent.current_index].name
        return "none"

    def set_root(self, root):
        self.current_node = root

    def update_node_name(self, up):
        # Define regex pattern to match "Link #" with any number between 1 and 3
        pattern = r"(.*)(link_([1-3]))(.*)"

        match = re.match(pattern, self.current_node.name)
        if match:
            # Extract the pre-text, current link number, and post-text
            pre_text, link_text, link_number, post_text = match.groups()

            self.log.trace(f"{link_text}")
            # Convert link_number to integer for calculation
            link_number = int(link_number)

            # Increment or decrement the link number based on `up` value
            if up:
                link_number = link_number + 1 if link_number < 3 else 1
            else:
                link_number = link_number - 1 if link_number > 1 else 3

            # Update self.current_node.name with the new text
            self.current_node.name = f"{pre_text}link_{link_number}{post_text}"
            print(f"updated name: {self.current_node.name}", flush=True)

    def on_up(self):
        if self.qdebug_on:
            print(f"on_up: current {self.current_node.name}", flush=True)
            if "link_" in self.current_node.name:
                self.update_node_name(True)
            self.print_current_page("onUp")

        return self.current_node.name if self.current_node else ""

    def on_down(self):
        if self.qdebug_on:
            print(f"on_down: current {self.current_node.name}", flush=True)
            if "link_" in self.current_node.name:
                self.update_node_name(False)
            self.print_current_page("onDown")

        return self.current_node.name if self.current_node else ""

    def on_right(self):
        if not self.current_node or not self.history:
            return ""

        parent = self.history[-1]
        if parent and parent.current_index < len(parent.children) - 1:
            parent.current_index += 1
            self.current_node = parent.children[parent.current_index]

        if self.qdebug_on:
            self.print_current_page("onRight")

        return self.current_node.name if self.current_node else ""

    def on_left(self):
        if not self.current_node or not self.history:
            return ""

        parent = self.history[-1]
        if parent and parent.current_index > 0:
            parent.current_index -= 1
            self.current_node = parent.children[parent.current_index]

        if self.qdebug_on:
            self.print_current_page("onLeft")

        return self.current_node.name if self.current_node else ""

    def on_enter(self, _print=True):
        if not self.current_node:
            return ""

        if self.current_node.children:
            self.history.append(self.current_node)

            if self.current_level == 2:
                self.current_feature = self.current_node.name

            self.current_node = self.current_node.children[
                self.current_node.current_index
            ]
            self.current_level += 1

        if self.qdebug_on and _print:
            self.print_current_page("onEnter")

        return self.current_node.name if self.current_node else ""

    def on_back(self):
        # print(f"self.current_level = {self.current_level}")
        if not self.current_node or not self.history or (self.current_level > 2):
            previous = self.history.pop()
            self.current_node = previous
            self.current_level -= 1

        if self.qdebug_on:
            self.print_current_page("onBack")

        return self.current_node.name if self.current_node else ""

    def print_current_page(self, info):
        if self.current_node:
            print(f"{info}: {self.current_node.name}")
