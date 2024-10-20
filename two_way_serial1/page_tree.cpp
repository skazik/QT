#include "page_tree.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <QDebug>

namespace page_tree {

PageNode::PageNode(std::string name) : name(std::move(name)) {}

void PageNode::addChild(std::unique_ptr<PageNode> child) {
    children.push_back(std::move(child));
}

void PageNode::printTree(int level) const {
    for (int i = 0; i < level; ++i) {
        std::cout << "  ";
    }
    std::cout << name << "\n";
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

void PageTree::printTree() const {
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

} //page_tree
