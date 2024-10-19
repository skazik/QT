from page_tree import PageTree


class Navigator:
    _instance = None

    def __new__(cls):
        if cls._instance is None:
            cls._instance = super().__new__(cls)
            cls._instance.history = []
            cls._instance.current_level = 0
            cls._instance.qdebug_on = True
            cls._instance.current_feature = ""

            # Initialize the Navigator with the root of the PageTree
            page_tree = PageTree()
            cls._instance.current_node = page_tree.root
            # advance to level 3
            cls._instance.on_enter()
            cls._instance.on_enter()
        return cls._instance

    def __init__(self):
        pass

    def get_current_feature(self):
        return self.current_feature

    def get_current_view_idx(self):
        parent = self.history[-1]
        if parent:
            return parent.current_index
        else:
            return -1

    def get_current_view_name(self):
        parent = self.history[-1]
        if parent:
            return parent.children[parent.current_index].name
        else:
            return -1

    def set_root(self, root):
        self.current_node = root

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

    def on_enter(self):
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

        if self.qdebug_on:
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
