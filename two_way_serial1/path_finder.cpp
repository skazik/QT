#include "page_tree.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <cassert>
#include <yaml-cpp/yaml.h>

static bool start_found = false;
static bool end_found = false;

///
/// \brief condense_rear_without_ok
/// \param processed_entries
///
void condense_rear_without_ok(std::vector<YAML::Node>& processed_entries) {
    std::vector<YAML::Node> condensed_entries;
    int last_rear_index = -1;  // Marker for the last "press rear button" command index
    bool ok_found = false;

    for (long unsigned int i = 0; i < processed_entries.size(); ++i) {
        const auto& entry = processed_entries[i];
        std::string current_command = entry["Command"] ? entry["Command"].as<std::string>() : "";

        // If we encounter an "press joystick button", reset the marker
        if (current_command == "press joystick button") {
            ok_found = true;
        }

        // If we encounter a "press rear button" and there was a previous "press rear button" without an "press joystick button" in between
        if (current_command == "press rear button") {
            if (last_rear_index != -1 && !ok_found) {
                // Remove all entries between the marked "press rear button" and this "press rear button"
                condensed_entries.resize(last_rear_index + 1);
            }

            // Add the current "press rear button" entry
            condensed_entries.push_back(entry);

            // Update the marker to the current index in the condensed list
            last_rear_index = condensed_entries.size() - 1;
            // Reset the ok_found flag for the next sequence
            ok_found = false;
        } else {
            // For non-"press rear button" commands, simply add the entry to the condensed list
            condensed_entries.push_back(entry);
        }
    }

    // Replace the original processed_entries with the condensed entries
    processed_entries = std::move(condensed_entries);
}


///
/// \brief condense_subnodes
/// \param processed_entries
/// \param parent_node
///
void condense_subnodes(std::vector<YAML::Node>& processed_entries, const std::string& parent_node) {
    std::vector<YAML::Node> condensed_entries;
    bool inside_parent = false;
    YAML::Node parent_entry;

    for (const auto& entry : processed_entries) {
        std::string current_node_name = entry["Postcondition"] ? entry["Postcondition"].as<std::string>() : "";
        std::string current_command = entry["Command"] ? entry["Command"].as<std::string>() : "";

        // Detect when entering the parent node
        if (current_node_name == parent_node && current_command == "navigate Right" && !inside_parent) {
            inside_parent = true;
            parent_entry = entry; // Store the entry when we first enter the parent
            continue;
        }

        // Detect when exiting the parent node
        if (inside_parent && current_command == "press rear button" && current_node_name == parent_node) {
            inside_parent = false;
            condensed_entries.push_back(parent_entry); // Add only the condensed entry
            continue;
        }

        // If we are not inside the parent, keep adding entries normally
        if (!inside_parent) {
            condensed_entries.push_back(entry);
        }
    }

    // Replace the original processed_entries with the condensed entries
    processed_entries = std::move(condensed_entries);
}


///
/// \brief find_repeated_node
/// \param processed_entries
/// \return
///
std::string find_repeated_node(const std::vector<YAML::Node>& processed_entries) {
    std::unordered_map<std::string, int> node_visit_count;

    for (const auto& entry : processed_entries) {
        std::string current_node_name = entry["Postcondition"] ? entry["Postcondition"].as<std::string>() : "";

        // Increase the visit count for the current node
        if (!current_node_name.empty()) {
            node_visit_count[current_node_name]++;
        }
    }

    // Find the first node that was visited more than once
    for (const auto& [node_name, count] : node_visit_count) {
        if (count > 1) {
            return node_name;  // Return the first repeated node
        }
    }

    return "";  // Return an empty string if no repeated node is found
}


///
/// \brief optimize_entries
/// \param processed_entries
/// \return
///
bool optimize_entries(std::vector<YAML::Node>& processed_entries) {
    std::vector<YAML::Node> optimized_entries;
    std::string last_command{};
    bool changes_made = false;

    for (const auto& entry : processed_entries) {
        // Extract the current command
        std::string current_command = entry["Command"] ? entry["Command"].as<std::string>() : "";

        if ((0 == last_command.compare("press joystick button")) && (0 == current_command.compare("press rear button"))) {
            // remove last entry with "press joystick button"
            optimized_entries.pop_back();
            // Update last seen command for the next iteration
            YAML::Node& last_entry = optimized_entries.back();
            last_command = last_entry["Command"].as<std::string>();
            // Indicate that a change was made and skip the current command
            changes_made = true;
            continue;
        }

        // Store the entry in the optimized list
        optimized_entries.push_back(entry);

        // Update last seen command for the next iteration
        last_command = current_command;
    }

    // Replace the original processed_entries with the optimized entries
    processed_entries = std::move(optimized_entries);

    return changes_made;
}


///
/// \brief process_entries
/// \param output_yaml
/// \param processed_entries
///
void process_entries(YAML::Node output_yaml, std::vector<YAML::Node>& processed_entries) {
    std::vector<YAML::Node> entries;

    // Iterate through the original YAML::Node
    for (auto entry : output_yaml) {
        // Store the modified entry in the vector for later reversal
        entries.push_back(entry);
    }

    // Print each entry's fields separately in the reversed order
    // Track the index and the total size for identifying the first and last entries
    std::size_t index = 0;
    for (const auto& entry : entries) {
        YAML::Node processed_entry;
        if (index == 0) {
            processed_entry["Command"] = "start";
        }
        else {
            processed_entry["Command"] = entry["Command"].as<std::string>();
        }
        processed_entry["Postcondition"] = entry["Postcondition"].as<std::string>();
        // Store the processed entry in the vector
        processed_entries.push_back(processed_entry);
        // Increment the index counter
        ++index;
    }
}


///
/// \brief check_start_end_and_save
/// \param node_name
/// \param start_name
/// \param end_name
/// \param output_yaml
/// \param cmd
///
void check_start_end_and_save(std::string node_name, std::string start_name, std::string end_name,
                              YAML::Node& output_yaml, const char* cmd) {
    bool done = false; // this is needed to add last found node (start or end) when all found
    YAML::Node entry;
    entry["Command"] = cmd;
    entry["Postcondition"] = node_name;
    if (start_found && end_found)
        return;
    if (0 == node_name.compare(start_name)) {
        if (start_found ^ end_found) {
            if (start_found) // if previously found - reset output_yaml
                output_yaml = YAML::Node();  // Reassign to an empty node
            output_yaml.push_back(entry); // save before updating start_found
            done = true;
        }
        start_found = true;
        if (done) return;
    }
    if (0 == node_name.compare(end_name)) {
        if (start_found ^ end_found) {
            if (end_found) // if previously found - reset output_yaml
                output_yaml = YAML::Node();  // Reassign to an empty node
            output_yaml.push_back(entry); // save before updating end_found
            done = true;
        }
        end_found = true;
        if (done) return;
    }
    if (start_found ^ end_found) output_yaml.push_back(entry);
}


///
/// \brief traverse_pagetree_recursive_path
/// \param currentNode
/// \param output_yaml
/// \param root_name
/// \param enter_on_right
/// \param start_name
/// \param end_name
///
void traverse_pagetree_recursive_path(PageNode* currentNode, YAML::Node& output_yaml,
                                      const char *root_name, bool enter_on_right,
                                      std::string start_name, std::string end_name) {
    assert(currentNode);

    // Process the current node as "press joystick button" when visiting it initially except "Main Menu"
    if (0 != currentNode->name.compare(root_name)) {
        check_start_end_and_save(currentNode->name, start_name, end_name, output_yaml, enter_on_right ? "navigate Right" : "press joystick button");
    }

    if (currentNode->children.empty()) {
        // No children - return back
        return;
    }

    // Recursively traverse through each child node
    bool _print_on_right = false;
    for (int index = 0; index < (int)currentNode->children.size(); _print_on_right = true, index++) {
        auto child = currentNode->children[index].get();

        // Recursive call for the child node
        traverse_pagetree_recursive_path(child, output_yaml, root_name, _print_on_right, start_name, end_name);
    }

    // Now return to itself
    if (0 != currentNode->name.compare(root_name)) {
        check_start_end_and_save(currentNode->name, start_name, end_name, output_yaml, "press rear button");
    }
}


///
/// \brief inspect_start_end
/// \param root
/// \param start_name
/// \param end_name
/// \param start_node
/// \param end_node
/// \param first_level_start_node
/// \param first_level_end_node
/// \return
///
bool inspect_start_end(PageNode* root,
                       std::string start_name,
                       std::string end_name,
                       PageNode** start_node,
                       PageNode** end_node,
                       PageNode** first_level_start_node,
                       PageNode** first_level_end_node) {
    if (!root || !first_level_start_node || !first_level_end_node) {
        return false;  // Return false if root or pointers are null
    }

    // Track if the nodes have been found and their respective order
    bool start_found = false;
    bool end_found = false;
    bool direct_order = false; // Default to false

    // Traverse the tree using a queue for breadth-first search
    std::queue<std::pair<PageNode*, PageNode*>> node_queue; // Pair of (current node, first level ancestor)
    for (auto& child : root->children) {
        node_queue.push({child.get(), child.get()}); // Each child is its own first-level ancestor
    }

    while (!node_queue.empty()) {
        auto [current_node, first_level_ancestor] = node_queue.front();
        node_queue.pop();

        // Check if we found the start_name or end_name
        if (current_node->name == start_name && !start_found) {
            start_found = true;
            *start_node = current_node;
            *first_level_start_node = first_level_ancestor;
        } else if (current_node->name == end_name && !end_found) {
            end_found = true;
            *end_node = current_node;
            *first_level_end_node = first_level_ancestor;
        }

        // Determine the order if both nodes have been found
        if (start_found && end_found) {
            direct_order = (current_node->name == end_name);
            break;
        }

        // Continue traversal by pushing children to the queue
        for (auto& child : current_node->children) {
            node_queue.push({child.get(), first_level_ancestor});
        }
    }

    return direct_order;
}


///
/// \brief traverse_pagetree_path
/// \param root
/// \param skip_root_name
/// \param start_name
/// \param end_name
///
void traverse_pagetree_path(PageNode* root, const char *skip_root_name, std::string start_name, std::string end_name) {
    if (!root) return;
    root = root->children[0].get();

    PageNode* first_level_start_node = nullptr;
    PageNode* first_level_end_node = nullptr;
    PageNode* start_node = nullptr;
    PageNode* end_node = nullptr;

    bool direct_order = inspect_start_end(root, start_name, end_name, &start_node, &end_node,
                                          &first_level_start_node, &first_level_end_node);
    assert(first_level_start_node);
    assert(first_level_end_node);
    assert(start_node);
    assert(end_node);

    bool debug_prefilled = false;

    if (debug_prefilled) {
        std::cout << "\n----------------------traverse debug-----------------------------\n";
        std::cout << "start_name: " << start_name << std::endl;
        std::cout << "end_name: " << end_name << std::endl;
        std::cout << "start_node: " << start_node->name << std::endl;
        std::cout << "First level start node: " << first_level_start_node->name << std::endl;
        std::cout << "end_node: " << end_node->name << std::endl;
        std::cout << "First level end node: " << first_level_end_node->name << std::endl << std::endl;
    }

    std::cout << (direct_order ? "direct_order" : "reverse_order") << std::endl;

    YAML::Node output_yaml;  // Store all

    // if reverse order
    if (!direct_order) {
        int end = first_level_start_node->node_level;
        int start = start_node->node_level;
        std::cout << "start level: " << start << " end level: " << end << " generate " << (start - end) << " commands\n\n";

        // create path from SN to FLSN
        {
            YAML::Node entry;
            entry["Command"] = "start";
            entry["Postcondition"] = start_node->name;
            output_yaml.push_back(entry);
        }
        for (int i = start-1; i >= end; --i) {
            YAML::Node entry;
            entry["Command"] = "press rear button";
            entry["Postcondition"] = ((i == end) ? first_level_start_node->name : "level " + std::to_string(i));
            output_yaml.push_back(entry);
        }

        // now traverse left from FLSN to the FLEN
        bool found = false;
        for (int i = root->children.size(); i > 0; --i) {
            if (found) {
                YAML::Node entry;
                entry["Command"] = "navigate Left";
                entry["Postcondition"] = root->children[i-1].get()->name;
                output_yaml.push_back(entry);
                if (root->children[i-1].get()->name == first_level_end_node->name) {
                    break;
                }
            } else if (root->children[i-1].get()->name == first_level_start_node->name) {
                found = true;
            }
        }

        if (debug_prefilled) {
            std::cout << "\n---------pre-filled for reverse-----------\n";
            for (std::size_t i = 0; i < output_yaml.size(); ++i) {
                 YAML::Node entry = output_yaml[i];
                 std::cout << entry << std::endl;
            }
            std::cout << "-------reverce done, back to direct -----------\n\n";
        }

        // from here we can use direct_order
        root = first_level_end_node;
        start_name = first_level_end_node->name;
    }

    // Start the recursive traversal from the root node
    YAML::Node direct_yanl;  // Store all YAML commands here
    start_found = false, end_found = false;
    traverse_pagetree_recursive_path(root, direct_yanl, skip_root_name, false, start_name, end_name);

    std::cout << "start: " << start_name << "\nend: " << end_name << "\n\n";
    std::vector<YAML::Node> processed_entries;
    process_entries(direct_yanl, processed_entries);
    while (optimize_entries(processed_entries));

    while (true) {
        std::string repeated_node = find_repeated_node(processed_entries);
        std::cout << "Repeated node: " << repeated_node << std::endl;
        if (!repeated_node.empty()) {
            condense_subnodes(processed_entries, repeated_node);
            continue;
        }
        break;
    }
    condense_rear_without_ok(processed_entries);

    // add all to output_yaml
    int idx = 0; // to skip first repeated entry in reverse
    for (const auto& entry : processed_entries) {
        if (!direct_order && !idx++) continue;
        output_yaml.push_back(entry);
    }

    // PRINT OUT THE FINAL
    for (const auto& entry : output_yaml) {
        if (entry["Command"]) {
            std::cout << "- Command: " << entry["Command"].as<std::string>() << std::endl;
        }
        if (entry["Postcondition"]) {
            std::cout << "  Postcondition: " << entry["Postcondition"].as<std::string>() << std::endl;
        }
    }
}

