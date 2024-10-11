#include "navigator.hpp"
#include "csv_reader.h"
#include "string.h"

namespace navigator {

Navigator::Navigator(csv_reader::PageNode* root)
    : currentNode(root) {}

void Navigator::setRoot(csv_reader::PageNode* root) {
    currentNode = root;
}

std::string Navigator::onRight() {
    if (!currentNode || history.empty())
        return "";
    csv_reader::PageNode *parent = history.back();
    if (parent && parent->currentIndex < parent->children.size() - 1 ) {
        currentNode = parent->children[++parent->currentIndex].get();
    }
    if (qdebug_on) printCurrentPage("onRight");
    return currentNode->name;
}

std::string Navigator::onLeft() {
    if (!currentNode || history.empty())
        return "";
    csv_reader::PageNode *parent = history.back();
    if (parent && parent->currentIndex > 0) {
        currentNode = parent->children[--parent->currentIndex].get();
    }
    if (qdebug_on) printCurrentPage("onLeft");
    return currentNode->name;
}

std::string Navigator::onEnter() {
    if (!currentNode)
        return "";
    if (currentNode && !currentNode->children.empty()) {
        history.push_back(currentNode); // Save current node to history
        csv_reader::PageNode * child = currentNode->children[currentNode->currentIndex].get();
        if (child) {
            currentNode = child;
            current_level++;
        }
    }
    if (qdebug_on) printCurrentPage("onEnter");
    return currentNode->name;
}

std::string Navigator::onBack() {
    if (!currentNode || history.empty())
        return "";
    if (!history.empty()) {
        currentNode = history.back(); // Go back to the previous node
        history.pop_back(); // Remove it from history
        current_level--;
    }
    if (qdebug_on) printCurrentPage("onBack");
    return currentNode->name;
}

void Navigator::printCurrentPage(const char *info) const {
    if (!currentNode)
        return;
    for (int i = 0; i < current_level; i++) {
        std::cout << "  ";
    }
    if (strlen(info)) {
        std::cout << info << " ";
    }
    if (currentNode) {
        // << " tabView: " << currentNode->children[currentNode->currentIndex].get()->name
        std::cout << currentNode->name << std::endl;
    } else {
        std::cout << "No current page." << std::endl;
    }
}

void test_navigator(Navigator& navigator) {
    navigator.onEnter(); // Main
    navigator.onEnter(); // B&R
    navigator.onEnter(); // segment
    navigator.onRight();  // no act
    navigator.onLeft(); // no act
    navigator.onEnter(); // Link1
    navigator.onBack();
    navigator.onBack();

    navigator.onRight(); // LED
    navigator.onRight(); // Shape S
        navigator.onEnter(); // Stright
        navigator.onRight(); // El1
        navigator.onRight(); // El2
        navigator.onRight(); // El3
        navigator.onRight(); // Stairs
        navigator.onRight(); // Stairs

    navigator.onBack(); // Shape S
    navigator.onRight();  // JControl
    navigator.onRight();  // Setup
    navigator.onRight();  // Information
    navigator.onLeft(); // Setup
        navigator.onEnter(); // Pack R
        navigator.onRight(); // Unpack
        navigator.onRight(); // Sys Connection
        navigator.onRight(); // Calibrate Joystick
        navigator.onRight(); // Robot Calibra
}

} // namespace navigator
