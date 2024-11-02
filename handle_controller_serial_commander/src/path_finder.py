import sys
from collections import deque

from logger import Logger


class PathFinder:
    _instance = None  # Class attribute to hold the singleton instance

    def __new__(cls):
        if cls._instance is None:
            cls._instance = super().__new__(cls)
        return cls._instance

    def __init__(self):
        if not hasattr(self, "log"):  # Ensure initialization runs only once
            self.log = Logger()
            self.start_found = False
            self.end_found = False

    def condense_rear_without_ok(self, processed_entries):
        condensed_entries = []
        last_rear_index = -1  # Marker for the last "press rear button" command index
        ok_found = False

        for entry in processed_entries:
            current_command = entry.get("Command", "")

            # If we encounter a "press joystick button", reset the marker
            if current_command == "press joystick button":
                ok_found = True

            # If we encounter a "press rear button" and there was a previous "press rear button" without an "OK" in between
            if current_command == "press rear button":
                if last_rear_index != -1 and not ok_found:
                    # Remove all entries between the marked "press rear button" and this "press rear button"
                    condensed_entries = condensed_entries[: last_rear_index + 1]

                # Add the current "press rear button" entry
                condensed_entries.append(entry)

                # Update the marker to the current index in the condensed list
                last_rear_index = len(condensed_entries) - 1
                # Reset the ok_found flag for the next sequence
                ok_found = False
            else:
                # For non-"press rear button" commands, simply add the entry to the condensed list
                condensed_entries.append(entry)

        # Replace the original processed_entries with the condensed entries
        processed_entries[:] = condensed_entries

    def condense_subnodes(self, processed_entries, parent_node):
        condensed_entries = []
        inside_parent = False
        parent_entry = None

        for entry in processed_entries:
            current_node_name = entry.get("Postcondition", "")
            current_command = entry.get("Command", "")

            # Detect when entering the parent node
            if (
                current_node_name == parent_node
                and current_command == "navigate Right"
                and not inside_parent
            ):
                inside_parent = True
                parent_entry = entry  # Store the entry when we first enter the parent
                continue

            # Detect when exiting the parent node
            if (
                inside_parent
                and current_command == "press rear button"
                and current_node_name == parent_node
            ):
                inside_parent = False
                condensed_entries.append(parent_entry)  # Add only the condensed entry
                continue

            # If we are not inside the parent, keep adding entries normally
            if not inside_parent:
                condensed_entries.append(entry)

        # Replace the original processed_entries with the condensed entries
        processed_entries[:] = condensed_entries

    def find_repeated_node(self, processed_entries):
        node_visit_count = {}

        for entry in processed_entries:
            current_node_name = entry.get("Postcondition", "")

            # Increase the visit count for the current node
            if current_node_name:
                node_visit_count[current_node_name] = (
                    node_visit_count.get(current_node_name, 0) + 1
                )

        # Find the first node that was visited more than once
        for node_name, count in node_visit_count.items():
            if count > 1:
                return node_name  # Return the first repeated node

        return ""  # Return an empty string if no repeated node is found

    def optimize_entries(self, processed_entries):
        optimized_entries = []
        last_command = ""
        changes_made = False

        for entry in processed_entries:
            # Extract the current command
            current_command = entry.get("Command", "")

            if (
                last_command == "press joystick button"
                and current_command == "press rear button"
            ):
                # Remove last entry with "press joystick button"
                optimized_entries.pop()
                # Update last seen command for the next iteration
                last_command = (
                    optimized_entries[-1].get("Command", "")
                    if optimized_entries
                    else ""
                )
                # Indicate that a change was made and skip the current command
                changes_made = True
                continue

            # Store the entry in the optimized list
            optimized_entries.append(entry)

            # Update last seen command for the next iteration
            last_command = current_command

        # Replace the original processed_entries with the optimized entries
        processed_entries[:] = optimized_entries

        return changes_made

    def process_entries(self, output_yaml, processed_entries):
        entries = []

        # Iterate through the original YAML node
        for entry in output_yaml:
            # Store the modified entry in the list for later reversal
            entries.append(entry)

        # Track the index for identifying the first entry
        for index, entry in enumerate(entries):
            processed_entry = {}
            if index == 0:
                processed_entry["Command"] = "start"
            else:
                processed_entry["Command"] = entry.get("Command", "")

            processed_entry["Postcondition"] = entry.get("Postcondition", "")

            # Store the processed entry in the processed_entries list
            processed_entries.append(processed_entry)

    def check_start_end_and_save(
        self, node_name, start_name, end_name, output_yaml, cmd
    ):
        done = False  # Needed to add last found node (start or end) when all found
        entry = {"Command": cmd, "Postcondition": node_name}

        if self.start_found and self.end_found:
            return

        if node_name == start_name:
            if self.start_found != self.end_found:
                if self.start_found:  # If previously found, reset output_yaml
                    output_yaml.clear()
                output_yaml.append(entry)  # Save before updating self.start_found
                done = True
            self.start_found = True
            if done:
                return

        if node_name == end_name:
            if self.start_found != self.end_found:
                if self.end_found:  # If previously found, reset output_yaml
                    output_yaml.clear()
                output_yaml.append(entry)  # Save before updating self.end_found
                done = True
            self.end_found = True
            if done:
                return

        if self.start_found != self.end_found:
            output_yaml.append(entry)

    def traverse_pagetree_recursive_path(
        self, current_node, output_yaml, root_name, enter_on_right, start_name, end_name
    ):
        assert current_node is not None

        # Process the current node as "press joystick button" when visiting it initially except "Main Menu"
        if current_node.name != root_name:
            self.check_start_end_and_save(
                current_node.name,
                start_name,
                end_name,
                output_yaml,
                "navigate Right" if enter_on_right else "press joystick button",
            )

        if not current_node.children:
            # No children - return back
            return

        # Recursively traverse through each child node
        _print_on_right = False
        for child in current_node.children:
            # Recursive call for the child node
            self.traverse_pagetree_recursive_path(
                child, output_yaml, root_name, _print_on_right, start_name, end_name
            )
            _print_on_right = True  # Set for subsequent children

        # Now return to itself
        if current_node.name != root_name:
            self.check_start_end_and_save(
                current_node.name,
                start_name,
                end_name,
                output_yaml,
                "press rear button",
            )

    def inspect_start_end(
        self,
        root,
        start_name,
        end_name,
        start_node,
        end_node,
        first_level_start_node,
        first_level_end_node,
    ):
        if (
            not root
            or not root.children
            or not start_node
            or not end_node
            or not first_level_start_node
            or not first_level_end_node
        ):
            return False  # Return false if root or any output pointers are null

        self.start_found = False
        self.end_found = False
        direct_order = False  # Default to false

        # Traverse the tree using a queue for breadth-first search
        node_queue = deque(
            [(child, child) for child in root.children]
        )  # Pair of (current node, first level ancestor)

        while node_queue:
            current_node, first_level_ancestor = node_queue.popleft()

            # Debug: Print each node as it's processed
            # self.log.debug(f"Checking node: {current_node.name}")

            if current_node.name == start_name and not self.start_found:
                self.start_found = True
                start_node[0] = current_node
                first_level_start_node[0] = first_level_ancestor
                self.log.debug(f"Found start_node: {start_node[0].name}")
            elif current_node.name == end_name and not self.end_found:
                self.end_found = True
                end_node[0] = current_node
                first_level_end_node[0] = first_level_ancestor
                self.log.debug(f"Found end_node: {end_node[0].name}")

            if self.start_found and self.end_found:
                direct_order = current_node.name == end_name
                break

            # Continue traversal by pushing children to the queue
            for child in current_node.children:
                node_queue.append((child, first_level_ancestor))

        return direct_order

    def traverse_pagetree_path(self, root, skip_root_name, start_name, end_name):
        if not root:
            return
        root = root.children[0]

        first_level_start_node = [None]
        first_level_end_node = [None]
        start_node = [None]
        end_node = [None]

        direct_order = self.inspect_start_end(
            root,
            start_name,
            end_name,
            start_node,
            end_node,
            first_level_start_node,
            first_level_end_node,
        )

        if start_node[0] is None:
            self.log.critical(f"Error: start_node with name '{start_name}' not found.")
            sys.exit()
        if end_node[0] is None:
            self.log.critical(f"Error: end_node with name '{end_name}' not found.")
            sys.exit()
        if first_level_start_node[0] is None or first_level_end_node[0] is None:
            self.log.critical("Error: Could not find start or end nodes in the tree.")
            sys.exit()

        debug_prefilled = False

        if debug_prefilled:
            self.log.debug(
                "\n----------------------traverse debug-----------------------------"
            )
            self.log.debug(f"start_name: {start_name}")
            self.log.debug(f"end_name: {end_name}")
            self.log.debug(f"start_node: {start_node[0].name}")
            self.log.debug(f"First level start node: {first_level_start_node[0].name}")
            self.log.debug(f"end_node: {end_node[0].name}")
            self.log.debug(f"First level end node: {first_level_end_node[0].name}\n")

        self.log.debug("direct_order" if direct_order else "reverse_order")

        output_yaml = []

        # if reverse order
        if not direct_order:
            end = first_level_start_node[0].level  # Access level of the actual PageNode
            start = start_node[0].level
            self.log.debug(
                f"start level: {start} end level: {end} generate {start - end} commands\n"
            )

            # create path from SN to FLSN
            output_yaml.append(
                {"Command": "start", "Postcondition": start_node[0].name}
            )
            for i in range(start - 1, end - 1, -1):
                output_yaml.append(
                    {
                        "Command": "press rear button",
                        "Postcondition": first_level_start_node[0].name
                        if i == end
                        else f"level {i}",
                    }
                )

            # now traverse left from FLSN to the FLEN
            found = False
            for i in range(len(root.children), 0, -1):
                child_name = root.children[i - 1].name
                if found:
                    output_yaml.append(
                        {"Command": "navigate Left", "Postcondition": child_name}
                    )
                    if child_name == first_level_end_node[0].name:
                        break
                elif child_name == first_level_start_node[0].name:
                    found = True

            if debug_prefilled:
                self.log.debug("\n---------pre-filled for reverse-----------")
                for entry in output_yaml:
                    self.log.debug(entry)
                self.log.debug("-------reverse done, back to direct -----------\n\n")

            # from here we can use direct_order
            root = first_level_end_node[0]
            start_name = first_level_end_node[0].name

        # Start the recursive traversal from the root node
        direct_yaml = []
        self.start_found, self.end_found = False, False
        self.traverse_pagetree_recursive_path(
            root, direct_yaml, skip_root_name, False, start_name, end_name
        )

        self.log.debug(f"start: {start_name}\nend: {end_name}\n")
        processed_entries = []
        self.process_entries(direct_yaml, processed_entries)
        while self.optimize_entries(processed_entries):
            pass

        while True:
            repeated_node = self.find_repeated_node(processed_entries)
            if repeated_node:
                self.log.trace(f"Repeated node: {repeated_node}")
                self.condense_subnodes(processed_entries, repeated_node)
                continue
            break
        self.condense_rear_without_ok(processed_entries)

        # add all to output_yaml
        for idx, entry in enumerate(processed_entries):
            if not direct_order and idx == 0:
                continue
            output_yaml.append(entry)

        self.log.debug("traverse_pagetree_path done\n")
        return output_yaml
