#ifndef PAGE_TREE_H
#define PAGE_TREE_H

#include <iostream>
#include <vector>
#include <memory>
#include <string>

// PageNode class declaration
class PageNode {
public:
    // Constructor
    PageNode(std::string name);

    // Method to add a child node
    void addChild(std::unique_ptr<PageNode> child);

    // Method to print the tree structure
    void printTree(int level = 0) const;

    std::string name;                        // Name of the page
    std::vector<std::unique_ptr<PageNode>> children;  // List of child nodes
    size_t currentIndex{0};                   // Current index of sub-pages
};

// PageTree class declaration
class PageTree {
public:
    // Constructor
    PageTree();

    // Method to parse CSV file and construct the tree
    bool parseCSV(const std::string& filepath);

    // Method to print the tree structure
    void printTree() const;

    PageNode* getRoot() const;  // Method to get the root node

private:
    std::unique_ptr<PageNode> root;  // Root node of the tree

    // Helper function to count leading commas to determine page level
    int countLeadingCommas(const std::string& line) const;
};

#endif // PAGE_TREE_H
