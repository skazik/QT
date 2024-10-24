#include "utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <iomanip>
#include <QDebug>

#include <string>
#include <cstdint>
#include <iostream>
#include <iomanip>

std::uint16_t simple_hash(const std::string& name) {
    std::uint32_t hash_value = 5381;  // Same initial value as in Python

    for (char c : name) {
        // Make sure we match the Python's (hash_value << 5) + hash_value + ord(c)
        hash_value = ((hash_value << 5) + hash_value) + static_cast<std::uint8_t>(c);
    }

    return static_cast<std::uint16_t>(hash_value & 0xFFFF);  // Ensure 16-bit result like Python
}

int test_hash_values() {
    // Example usage
    std::string names[] = {
        "Root",
        "Main Menu",
        "Bend & Rotate",
        "Bend & Rotate Segment",
        "Link 1 Bend & Rotate",
        "LED Lights",
        "Brightness",
        "Shape Selection",
        "Straight",
        "Straight Selected",
        "Elbow 1",
        "Elbow 1 Selected",
        "Elbow 2",
        "Elbow 2 Selected",
        "Elbow 3",
        "Elbow 3 Selected",
        "Stairstep",
        "Stairstep Selected",
        "Joint Control",
        "Joint Control Segment",
        "Link 1 Joint Control",
        "Setup",
        "Pack Robot",
        "Pack Robot Selected",
        "Unpack Robot",
        "Unpack Robot Selected",
        "System Connection",
        "System Connection Selected",
        "Calibrate Joystick",
        "Calibrate Joystick Selected",
        "Robot Calibration",
        "Home Segments",
        "Home Segments Select",
        "Move to Straight",
        "Move to Straight Segment",
        "Link 1 Move to Straight",
        "Set Position",
        "Reset Position",
        "Information",
        "System Information"
    };
    for (const auto& name : names) {
        std::uint16_t hash_value = simple_hash(name);
        std::cout << std::hex << std::uppercase << std::setw(4) << std::setfill('0')
                  << hash_value << " " << name << std::endl;
    }
    return 0;
}

//    test_hash_values();

void PageNode::addChild(std::unique_ptr<PageNode> child) {
    children.push_back(std::move(child));
}

void PageNode::printTree(int level) const {
    this->node_level = level;
    std::uint16_t hash_value = simple_hash(name);
    std::cout << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << hash_value;
    std::cout << "| " << node_level;

    for (int i = 0; i < level; ++i) {
        std::cout << "  ";
    }
    std::cout << " " << name << std::endl;

    for (const auto& child : children) {
        child->printTree(level + 1);
    }
    std::cout.flush();
}

PageTree::PageTree() {
    root = std::make_unique<PageNode>("Root");
}

std::string removeEndingCommas(const std::string& pageName) {
    // Find the last character that is not a comma
    size_t end = pageName.find_last_not_of(',');

    // If all characters are commas, return an empty string
    if (end == std::string::npos) {
        return "";
    }

    // Return the substring up to the last non-comma character
    return pageName.substr(0, end + 1);
}

PageNode* PageTree::getRoot() const {
    return root.get(); // Return the raw pointer to the root node
}

bool PageTree::parseCSV(const std::string& filepath) {
    std::ifstream file(filepath);

    if (!file.is_open()) {
        qDebug() << "Error: Unable to open file " << filepath.c_str();
//        throw std::runtime_error("Failed to open file: " + filepath); // Optional: throw an exception
        return false;
    }

    std::string line;

    std::vector<PageNode*> stack;
    stack.push_back(root.get());

    while (std::getline(file, line)) {
//        qDebug() << line.c_str() << endl;
        std::string pageName;
        size_t level = countLeadingCommas(line);
        pageName = line.substr(level);  // Page name is after the commas
        pageName = removeEndingCommas(pageName);
//        qDebug() << level << " " << pageName.c_str();

        auto newNode = std::make_unique<PageNode>(pageName);
        PageNode* currentNode = stack[level];

        // Ensure the stack is correctly sized for the current level
        if (stack.size() > level + 1) {
            stack.resize(level + 1);
        }

        // Add the node to the correct parent
        currentNode->addChild(std::move(newNode));

        // Add new node to stack
        stack.push_back(currentNode->children.back().get());
    }
    return true;
}

bool PageTree::parseYAML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        qDebug() << "Error: Unable to open file " << filepath.c_str();
        return false;
    }

    YAML::Node yamlData = YAML::LoadFile(filepath);

    // Function to recursively create PageNodes from YAML
    std::function<void(const YAML::Node&, PageNode*)> parseNode = [&](const YAML::Node& node, PageNode* parentNode) {
        std::string pageName = node["page_display_name"].as<std::string>();
        auto newNode = std::make_unique<PageNode>(pageName);

        // Add the new node to its parent
        parentNode->addChild(std::move(newNode));
        PageNode* currentNode = parentNode->children.back().get();

        // If the current node has sub-pages, recursively parse them
        if (node["sub_pages"]) {
            for (const auto& subPage : node["sub_pages"]) {
                parseNode(subPage, currentNode);
            }
        }
    };

    // Start parsing from the root node of the YAML structure
    parseNode(yamlData, root.get());

    return true;
}

void PageTree::printTree() {
    root->printTree();
}

int PageTree::countLeadingCommas(const std::string& line) const {
    int count = 0;
    for (char ch : line) {
        if (ch == ',') {
            ++count;
        } else {
            break;
        }
    }
    return count;
}
