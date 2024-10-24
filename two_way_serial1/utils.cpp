#include "utils.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <unordered_set>
#include <unordered_map>
#include <queue>

// Function to trim whitespace from the start and end of a string
std::string trim(const std::string& str) {
    auto start = std::find_if_not(str.begin(), str.end(), [](unsigned char ch) {
        return std::isspace(ch);
    });
    auto end = std::find_if_not(str.rbegin(), str.rend(), [](unsigned char ch) {
        return std::isspace(ch);
    }).base();
    return (start < end ? std::string(start, end) : "");
}

// Function to trim leading whitespace from each line
std::string trimLeadingWhitespaceFromEachLine(const std::string& str) {
    std::istringstream stream(str);
    std::string line;
    std::string result;

    while (std::getline(stream, line)) {
        result += trim(line) + "\n";
    }

    return result;
}

void copy_to_file(const std::string& dest) {
    // Check if the destination file exists
    if (std::filesystem::exists(dest)) {
        // Attempt to remove the existing file
        if (!std::filesystem::remove(dest)) {
            std::cerr << "Failed to remove existing file: " << dest << std::endl;
            return;
        }
    }

    // Copy the file to the destination
    try {
        std::filesystem::copy("inOutFileTmp.txt", dest, std::filesystem::copy_options::overwrite_existing);
        std::cout << "File copied successfully!" << std::endl;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Failed to copy file: " << e.what() << std::endl;
    }
}

void copy_from_file(const std::string& src) {
    std::string dest = "inOutFileTmp.txt";
    // Close the file if it's open (handled in actual use case)

    // Check if the destination file exists
    if (std::filesystem::exists(dest)) {
        // Attempt to remove the existing file
        if (!std::filesystem::remove(dest)) {
            std::cerr << "Failed to remove existing file: " << dest << std::endl;
            return;
        }
    }

    // Copy the file to the destination
    try {
        std::filesystem::copy(src, dest, std::filesystem::copy_options::overwrite_existing);
        // std::cout << "File copied successfully!" << std::endl;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Failed to copy file: " << src << std::endl;
    }
}

void traverse_pagetree_recursive_json(PageNode* currentNode, json& output_json, const char *root_name, bool enter_on_right) {
    assert(currentNode);

    // Process the current node as "OK" when visiting it initially except "Main Menu"
    if (0 != currentNode->name.compare(root_name)) {
        json entry = {
            {"command", enter_on_right ? "Right" : "OK"},
            {"timeout", "1"},
            {"expected", currentNode->name}
        };
        output_json.push_back(entry);
        std::cout << entry.dump() << std::endl << std::flush;
    }

    if (currentNode->children.size() <= 0) {
        // no children - return back
        return;
    }

    // Recursively traverse through each child node
    bool _print_on_right = false;
    for (int index = 0; index < (int) currentNode->children.size(); _print_on_right = true, index++) {
        auto child = currentNode->children[index].get();

        // Recursive call for the child node
        traverse_pagetree_recursive_json(child, output_json, root_name, _print_on_right);
    }

    // here we are at the last child - then go left to the first child
    for (int index = currentNode->children.size()-2; index >= 0; index--) {
        json leftEntry = {
            {"command", "Left"},
            {"timeout", "1"},
            {"expected", currentNode->children[index].get()->name}
        };
        output_json.push_back(leftEntry);
        std::cout << leftEntry.dump() << std::endl << std::flush;
    }

    // now print return to self
    if (0 != currentNode->name.compare(root_name)){
        json rearEntry = {
            {"command", "Rear"},
            {"timeout", "1"},
            {"expected", currentNode->name}
        };
        output_json.push_back(rearEntry);
        std::cout << rearEntry.dump() << std::endl << std::flush;
    }
}

void traverse_pagetree_json(PageNode* root, const char *root_name) {
    json output_json = json::array();  // Store all JSON commands here

    if (!root) return;
    root = root->children[0].get();

    // Start the recursive traversal from the root node
    traverse_pagetree_recursive_json(root, output_json, root_name);

    // don't write the output JSON to the file
    // outfile << output_json.dump(4);
    // Convert the JSON array to a string
    std::string json_str = output_json.dump(4); // Pretty-print with 4 spaces

    // Remove the first '[' and the last ']' and their newlines
    if (!json_str.empty() && json_str.front() == '[' && json_str.back() == ']') {
        json_str = json_str.substr(1, json_str.size() - 2);
    }

    // Trim leading or trailing whitespace
    json_str = trimLeadingWhitespaceFromEachLine(json_str);

    std::ofstream outfile("tmp_traverse_test.json");
    if (outfile.is_open()) {
        outfile<< json_str;
        outfile.close();
    } else {
        std::cerr << "Failed to open json file for writing" << std::endl;
    }
}

void traverse_pagetree_recursive_yaml(PageNode* currentNode, YAML::Node& output_yaml, const char *root_name, bool enter_on_right) {
    assert(currentNode);

    // Process the current node as "OK" when visiting it initially except "Main Menu"
    if (0 != currentNode->name.compare(root_name)) {
        YAML::Node entry;
        entry["command"] = enter_on_right ? "Right" : "OK";
        entry["timeout"] = "1";
        entry["expected"] = currentNode->name;

        output_yaml.push_back(entry);
        std::cout << "-" << entry << std::endl;  // Output the YAML entry to stdout
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
        traverse_pagetree_recursive_yaml(child, output_yaml, root_name, _print_on_right);
    }

    // Traverse left back to the first child
    for (int index = currentNode->children.size() - 2; index >= 0; index--) {
        YAML::Node leftEntry;
        leftEntry["command"] = "Left";
        leftEntry["timeout"] = "1";
        leftEntry["expected"] = currentNode->children[index].get()->name;

        output_yaml.push_back(leftEntry);
        std::cout << "-" << leftEntry << std::endl;  // Output the YAML entry to stdout
    }

    // Now print return to self
    if (0 != currentNode->name.compare(root_name)) {
        YAML::Node rearEntry;
        rearEntry["command"] = "Rear";
        rearEntry["timeout"] = "1";
        rearEntry["expected"] = currentNode->name;

        output_yaml.push_back(rearEntry);
        std::cout << "-" << rearEntry << std::endl;  // Output the YAML entry to stdout
    }
}

void traverse_pagetree_yaml(PageNode* root, const char *root_name) {
    YAML::Node output_yaml;  // Store all YAML commands here

    if (!root) return;
    root = root->children[0].get();

    // Start the recursive traversal from the root node
    traverse_pagetree_recursive_yaml(root, output_yaml, root_name);

    // Convert the YAML node to a string
    std::stringstream yaml_stream;
    yaml_stream << output_yaml;

    std::string yaml_str = yaml_stream.str();

    // Trim leading or trailing whitespace
    yaml_str = trimLeadingWhitespaceFromEachLine(yaml_str);

    std::ofstream outfile("tmp_traverse_test.yaml");
    if (outfile.is_open()) {
        outfile << yaml_str;
        outfile.close();
    } else {
        std::cerr << "Failed to open YAML file for writing" << std::endl;
    }
}

////////////////////////////////// experimental ///////////////////////
/// \brief start_found
///
///
static bool start_found = false, end_found = false;
void check_start_end_and_save(std::string node_name, std::string start_name, std::string end_name,
                              YAML::Node& output_yaml, const char* cmd) {
    bool done = false; // this is needed to add last found node (start or end) when all found
    YAML::Node entry;
    entry["command"] = cmd;
    entry["node_name"] = node_name;
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

void traverse_pagetree_recursive_path(PageNode* currentNode, YAML::Node& output_yaml,
                                      const char *root_name, bool enter_on_right,
                                      std::string start_name, std::string end_name) {
    assert(currentNode);

    // Process the current node as "OK" when visiting it initially except "Main Menu"
    if (0 != currentNode->name.compare(root_name)) {
        check_start_end_and_save(currentNode->name, start_name, end_name, output_yaml, enter_on_right ? "Right" : "OK");
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
        check_start_end_and_save(currentNode->name, start_name, end_name, output_yaml, "Rear");
    }
}

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
            processed_entry["command"] = "start";
        }
        else {
            processed_entry["command"] = entry["command"].as<std::string>();
        }
        processed_entry["node_name"] = entry["node_name"].as<std::string>();
        // Store the processed entry in the vector
        processed_entries.push_back(processed_entry);
        // Increment the index counter
        ++index;
    }
}

bool optimize_entries(std::vector<YAML::Node>& processed_entries) {
    std::vector<YAML::Node> optimized_entries;
    std::string last_command{};
    bool changes_made = false;

    for (const auto& entry : processed_entries) {
        // Extract the current command
        std::string current_command = entry["command"] ? entry["command"].as<std::string>() : "";

        if ((0 == last_command.compare("OK")) && (0 == current_command.compare("Rear"))) {
            // remove last entry with "OK"
            optimized_entries.pop_back();
            // Update last seen command for the next iteration
            YAML::Node& last_entry = optimized_entries.back();
            last_command = last_entry["command"].as<std::string>();
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

void condense_subnodes(std::vector<YAML::Node>& processed_entries, const std::string& parent_node) {
    std::vector<YAML::Node> condensed_entries;
    bool inside_parent = false;
    YAML::Node parent_entry;

    for (const auto& entry : processed_entries) {
        std::string current_node_name = entry["node_name"] ? entry["node_name"].as<std::string>() : "";
        std::string current_command = entry["command"] ? entry["command"].as<std::string>() : "";

        // Detect when entering the parent node
        if (current_node_name == parent_node && current_command == "Right" && !inside_parent) {
            inside_parent = true;
            parent_entry = entry; // Store the entry when we first enter the parent
            continue;
        }

        // Detect when exiting the parent node
        if (inside_parent && current_command == "Rear" && current_node_name == parent_node) {
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

std::string find_repeated_node(const std::vector<YAML::Node>& processed_entries) {
    std::unordered_map<std::string, int> node_visit_count;

    for (const auto& entry : processed_entries) {
        std::string current_node_name = entry["node_name"] ? entry["node_name"].as<std::string>() : "";

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

void condense_rear_without_ok(std::vector<YAML::Node>& processed_entries) {
    std::vector<YAML::Node> condensed_entries;
    int last_rear_index = -1;  // Marker for the last "Rear" command index
    bool ok_found = false;

    for (long unsigned int i = 0; i < processed_entries.size(); ++i) {
        const auto& entry = processed_entries[i];
        std::string current_command = entry["command"] ? entry["command"].as<std::string>() : "";

        // If we encounter an "OK", reset the marker
        if (current_command == "OK") {
            ok_found = true;
        }

        // If we encounter a "Rear" and there was a previous "Rear" without an "OK" in between
        if (current_command == "Rear") {
            if (last_rear_index != -1 && !ok_found) {
                // Remove all entries between the marked "Rear" and this "Rear"
                condensed_entries.resize(last_rear_index + 1);
            }

            // Add the current "Rear" entry
            condensed_entries.push_back(entry);

            // Update the marker to the current index in the condensed list
            last_rear_index = condensed_entries.size() - 1;
            // Reset the ok_found flag for the next sequence
            ok_found = false;
        } else {
            // For non-"Rear" commands, simply add the entry to the condensed list
            condensed_entries.push_back(entry);
        }
    }

    // Replace the original processed_entries with the condensed entries
    processed_entries = std::move(condensed_entries);
}

////////////////////////////////////// new //////////////////////////////
/// \brief inspect_start_end
/// \param root
/// \param start_name
/// \param end_name
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

    std::cout << "start_name: " << start_name << std::endl;
    std::cout << "end_name: " << end_name << std::endl;
    std::cout << (direct_order ? "direct_order" : "reverse_order") << std::endl << std::endl;
    std::cout << "start_node: " << start_node->name << std::endl;
    std::cout << "First level start node: " << first_level_start_node->name << std::endl << std::endl;
    std::cout << "end_node: " << end_node->name << std::endl;
    std::cout << "First level end node: " << first_level_end_node->name << std::endl << std::endl;

    YAML::Node output_yaml;  // Store all

    // if reverse order
    if (!direct_order) {
        int end = first_level_start_node->node_level;
        int start = start_node->node_level;
        std::cout << "start level: " << start << " end level: " << end << " generate " << (start - end) << " commands\n\n";

        // create path from SN to FLSN
        {
            YAML::Node entry;
            entry["command"] = "start";
            entry["node_name"] = start_node->name;
            output_yaml.push_back(entry);
        }
        for (int i = start-1; i >= end; --i) {
            YAML::Node entry;
            entry["command"] = "Rear";
            entry["node_name"] = ((i == end) ? first_level_start_node->name : "level " + std::to_string(i));
            output_yaml.push_back(entry);
        }

        // now traverse left from FLSN to the FLEN
        bool found = false;
        for (int i = root->children.size(); i > 0; --i) {
            if (found) {
                YAML::Node entry;
                entry["command"] = "Left";
                entry["node_name"] = root->children[i-1].get()->name;
                output_yaml.push_back(entry);
                if (root->children[i-1].get()->name == first_level_end_node->name) {
                    break;
                }
            } else if (root->children[i-1].get()->name == first_level_start_node->name) {
                found = true;
            }
        }

        std::cout << "\n---------pre-filled for reverse-----------\n";
        for (std::size_t i = 0; i < output_yaml.size(); ++i) {
             YAML::Node entry = output_yaml[i];
             std::cout << entry << std::endl;
        }
        std::cout << "-------reverce done, back to direct -----------\n\n";

        // from here we can use direct_order
        root = first_level_end_node;
        start_name = first_level_end_node->name;
    }
//    else {
//        root = first_level_start_node;
//        start_name = first_level_start_node->name;
//    }

    // Start the recursive traversal from the root node
    YAML::Node direct_yanl;  // Store all YAML commands here
    start_found = false, end_found = false;
    traverse_pagetree_recursive_path(root, direct_yanl, skip_root_name, false, start_name, end_name);

    std::cout << "start: " << start_name << "\nend: " << end_name << std::endl;
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
        if (entry["command"]) {
            std::cout << "- command: " << entry["command"].as<std::string>() << std::endl;
        }
        if (entry["node_name"]) {
            std::cout << "  node_name: " << entry["node_name"].as<std::string>() << std::endl;
        }
    }
}

