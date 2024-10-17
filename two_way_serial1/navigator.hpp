#ifndef NAVIGATOR_HPP
#define NAVIGATOR_HPP

#include "csv_reader.h"
#include <vector>
#include <stack>

namespace navigator {

// Navigator class declaration
class Navigator {
public:
    Navigator() {}
    // Constructor that takes the root node of the PageTree
    Navigator(csv_reader::PageNode* root);

    void setRoot(csv_reader::PageNode* root);

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
    csv_reader::PageNode* currentNode{nullptr}; // Pointer to the current node
    std::vector<csv_reader::PageNode*> history; // Stack of visited nodes
    int current_level{0};
    bool qdebug_on{false};
};

void test_navigator(Navigator& navigator);

} // namespace csv_reader

#endif // NAVIGATOR_HPP
