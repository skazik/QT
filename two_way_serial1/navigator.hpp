#ifndef NAVIGATOR_HPP
#define NAVIGATOR_HPP

#include "page_tree.h"
#include <vector>
#include <stack>

namespace navigator {

// Navigator class declaration
class Navigator {
public:
    Navigator() {}
    // Constructor that takes the root node of the PageTree
    Navigator(page_tree::PageNode* root);

    void setRoot(page_tree::PageNode* root);

    // Method to go to the next page on the same level
    std::string onRight();

    // Method to go to the previous page on the same level
    std::string onLeft();

    // Method to go to sub-level if exists
    std::string onEnter();

    // Method to return to the previous level if exists
    std::string onBack();

    // Method to print the current page name
    void printCurrentPage(const char *info = "") const;

    int get_current_level() { return current_level; }
    std::string get_current_parent() { return history.back()->name; }

private:
    page_tree::PageNode* currentNode{nullptr}; // Pointer to the current node
    std::vector<page_tree::PageNode*> history; // Stack of visited nodes
    int current_level{0};
    bool qdebug_on{false};
};

void test_navigator(Navigator& navigator);

} // namespace page_tree

#endif // NAVIGATOR_HPP
