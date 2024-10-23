#include "utils.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <unordered_set>

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

static bool start_found = false, end_found = false, direct_order = true;
bool to_store() {
    return (start_found ^ end_found);
}
void check_start_end(std::string node_name, const char* start_name, const char* end_name,YAML::Node& output_yaml, YAML::Node& entry) {
    bool done = false;
    if (start_found && end_found)
        return;
    if (0 == node_name.compare(start_name)) {
        if (to_store()) {
            if (start_found) // if previously found - reset output_yaml
                output_yaml = YAML::Node();  // Reassign to an empty node
            output_yaml.push_back(entry);
            done = true;
        }
        start_found = true;
        if (done) return;
    }
    if (0 == node_name.compare(end_name)) {
        if (to_store()) {
            if (end_found) // if previously found - reset output_yaml
                output_yaml = YAML::Node();  // Reassign to an empty node
            output_yaml.push_back(entry);
            done = true;
        }  else {
            direct_order = false;
        }
        end_found = true;
        if (done) return;
    }
    if (to_store()) output_yaml.push_back(entry);
}

void traverse_pagetree_recursive_path(PageNode* currentNode, YAML::Node& output_yaml, const char *root_name, bool enter_on_right, const char* start_name, const char* end_name) {
    assert(currentNode);

    // Process the current node as "OK" when visiting it initially except "Main Menu"
    if (0 != currentNode->name.compare(root_name)) {
        YAML::Node entry;
        entry["command"] = enter_on_right ? "Right" : "OK";
        entry["node_name"] = currentNode->name;

        check_start_end(currentNode->name, start_name, end_name, output_yaml, entry);
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
        YAML::Node entry;
        entry["command"] = "Rear";
        entry["node_name"] = currentNode->name;

        check_start_end(currentNode->name, start_name, end_name, output_yaml, entry);
    }
}

void print_yaml_in_reverse(YAML::Node output_yaml) {
    std::vector<YAML::Node> entries;
    std::vector<YAML::Node> processed_entries;

    // Iterate through the original YAML::Node
    for (auto entry : output_yaml) {
        // rename command to oposite
        if (entry["command"]) {
            std::string command = entry["command"].as<std::string>();
            if (command == "Rear") {
                entry["command"] = "OK";
            } else if (command == "OK") {
                entry["command"] = "Rear";
            } else if (command == "Right") {
                entry["command"] = "Left";
            }
        }
        // Store the modified entry in the vector for later reversal
        entries.push_back(entry);
    }

    // Reverse the vector of YAML nodes
    std::reverse(entries.begin(), entries.end());

    // Print each entry's fields separately in the reversed order
    // Track the index and the total size for identifying the first and last entries
    std::size_t total_entries = entries.size();
    std::size_t index = 0;
    YAML::Node tmp_entry;
    for (const auto& entry : entries) {
        YAML::Node processed_entry;

        // Check if it's the first entry
        bool is_first = (index == 0);
        // Check if it's the last entry
        bool is_last = (index == total_entries - 1);

        if (is_first) {
            processed_entry["command"] = "start";
            if (entry["node_name"]) {
                processed_entry["node_name"] = entry["node_name"].as<std::string>();
            } else {
                processed_entry["node_name"] = "-error-";
            }
            if (entry["command"]) {
                tmp_entry["command"] = entry["command"].as<std::string>();
            } else {
                tmp_entry["command"] = "-error-";
            }
        }
        else if (is_last) {
            processed_entry["command"] = "end";
            if (entry["node_name"]) {
                processed_entry["node_name"] = entry["node_name"].as<std::string>();
            } else {
                processed_entry["node_name"] = "-error-";
            }
        }
        else {
            processed_entry["command"] = tmp_entry["command"].as<std::string>();
            if (entry["node_name"]) {
                processed_entry["node_name"] = entry["node_name"].as<std::string>();
            } else {
                processed_entry["node_name"] = "-error-";
            }
            if (entry["command"]) {
                tmp_entry["command"] = entry["command"].as<std::string>();
            } else {
                tmp_entry["command"] = "-error-";
            }
        }

        // Store the processed entry in the vector
        processed_entries.push_back(processed_entry);
        // Increment the index counter
        ++index;
    }
    // Now output the processed entries in the desired format
    for (const auto& entry : processed_entries) {
        if (entry["command"]) {
            std::cout << "- command: " << entry["command"].as<std::string>() << std::endl;
        }
        if (entry["node_name"]) {
            std::cout << "  node_name: " << entry["node_name"].as<std::string>() << std::endl;
        }
    }
}

void giigle_print_yaml_in_reverse(YAML::Node output_yaml) {
    std::vector<YAML::Node> entries;
    std::vector<YAML::Node> processed_entries;

    // Iterate through the original YAML::Node and modify commands
    for (auto entry : output_yaml) {
        if (entry["command"]) {
            std::string command = entry["command"].as<std::string>();
            if (command == "Rear") {
                entry["command"] = "OK";
            } else if (command == "OK") {
                entry["command"] = "Rear";
            } else if (command == "Right") {
                entry["command"] = "Left";
            }
        }
        entries.push_back(entry);
    }

    // Reverse the vector of YAML nodes
    std::reverse(entries.begin(), entries.end());

    // Track the index and total size for identifying first and last entries
    std::size_t total_entries = entries.size();
    std::size_t index = 0;

    // Iterate over the reversed entries
    for (const auto& entry : entries) {
        YAML::Node processed_entry;

        // Check if it's the first entry
        bool is_first = (index == 0);
        // Check if it's the last entry
        bool is_last = (index == total_entries - 1);

        if (is_first) {
            processed_entry["command"] = "start";
        } else if (is_last) {
            processed_entry["command"] = "end";
        }

        // Add node_name if it exists
        if (entry["node_name"]) {
            processed_entry["node_name"] = entry["node_name"].as<std::string>();
        }

        // Add the command unless it's the last entry
        if (!is_last && entry["command"]) {
            processed_entry["command"] = entry["command"].as<std::string>();
        }

        // Store the processed entry in the vector
        processed_entries.push_back(processed_entry);

        // Increment the index counter
        ++index;
    }

    // Now output the processed entries in the desired format
    for (const auto& entry : processed_entries) {
        if (entry["command"]) {
            std::cout << "- command: " << entry["command"].as<std::string>() << std::endl;
        }
        if (entry["node_name"]) {
            std::cout << "  node_name: " << entry["node_name"].as<std::string>() << std::endl;
        }
    }
}


void traverse_pagetree_path(PageNode* root, const char *root_name, const char* start_name, const char* end_name) {
    YAML::Node output_yaml;  // Store all YAML commands here

    if (!root) return;
    root = root->children[0].get();

    // Start the recursive traversal from the root node
    start_found = false, end_found = false, direct_order = true;
    traverse_pagetree_recursive_path(root, output_yaml, root_name, false, start_name, end_name);

    std::cout << "start: " << start_name << "\nend: " << end_name << std::endl;
    if (direct_order) {
        std::stringstream yaml_stream;
        yaml_stream << output_yaml;
        std::string yaml_str = yaml_stream.str();
        std::cout << yaml_str << std::endl;
    } else {
        print_yaml_in_reverse(output_yaml);
    }
}

