#ifndef NAVIGATOR_HPP
#define NAVIGATOR_HPP

#include <iostream>
#include <string>
#include <vector>

// Struct representing a Page
struct Page {
    std::string page_name;
    std::string page_display_name;
    int num_sub_pages;
    std::vector<Page> sub_pages;

    // Constructor for easy page initialization
    Page(const std::string& name, const std::string& display_name, int num_subs)
        : page_name(name), page_display_name(display_name), num_sub_pages(num_subs) {}

    // Add sub-pages to the page
    void addSubPage(const Page& sub_page) {
        sub_pages.push_back(sub_page);
    }
};

// Class for navigation management
class PageNavigator {
public:
    PageNavigator(Page& root)
        : current_page(&root), current_index(0) {}

    // Navigate to the previous page (LEFT)
    void navigateLeft() {
//        if (current_index > 0) {
//            --current_index;
//            std::cout << "Navigated LEFT to: " << current_page->sub_pages[current_index].page_display_name << std::endl;
//        } else {
//            std::cout << "Already at the first page in this level!" << std::endl;
//        }
    }

    // Navigate to the next page (RIGHT)
    void navigateRight() {
//        if (current_page->sub_pages.empty()) {
//            std::cout << "No sub-pages at this level!" << std::endl;
//            return;
//        }

//        if (current_index < current_page->sub_pages.size() - 1) {
//            ++current_index;
//            std::cout << "Navigated RIGHT to: " << current_page->sub_pages[current_index].page_display_name << std::endl;
//        } else {
//            std::cout << "Already at the last page in this level!" << std::endl;
//        }
    }

    // Enter a sub-page (ENTER)
    void navigateEnter() {
//        if (!current_page->sub_pages.empty()) {
//            parent_pages.push_back(current_page);
//            index_stack.push_back(current_index);

//            current_page = &current_page->sub_pages[current_index];
//            current_index = 0;

////            std::cout << "Entered: " << current_page->page_display_name << std::endl;
//            displayCurrentPage();
//        } else {
//            std::cout << "No sub-pages to enter!" << std::endl;
//        }
    }

    // Go back to the previous level (BACK)
    void navigateBack() {
//        if (!parent_pages.empty()) {
//            current_page = parent_pages.back();
//            current_index = index_stack.back();

//            parent_pages.pop_back();
//            index_stack.pop_back();

////            std::cout << "Navigated BACK to: " << current_page->page_display_name << std::endl;
//            displayCurrentPage();
//        } else {
//            std::cout << "Already at the root page!" << std::endl;
//        }
    }

    // Display current page info
    void displayCurrentPage() const {
        std::cout << "Current Page: " << current_page->page_display_name;
        if (current_page->num_sub_pages > 0) {
            std::cout << ", tabview: " << current_page->sub_pages[current_index].page_display_name;
        } else {
            std::cout << " (" << current_page->page_name << ")";
        }
        std::cout << std::endl;
    }

private:
    Page* current_page;                // Pointer to the current page
    size_t current_index;              // Index in the current page's sub-pages

    std::vector<Page*> parent_pages;   // Stack to keep track of parent pages for BACK navigation
    std::vector<int> index_stack;      // Stack to keep track of the index at each level
};


void navigator_init();
PageNavigator* navigator_get();
void displayPages(const Page& page, int indent = 0);

#endif // NAVIGATOR_HPP
