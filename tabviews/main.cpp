#include <iostream>
#include <string>
#include <vector>

#if 2
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
        if (current_index > 0) {
            --current_index;
            std::cout << "Navigated LEFT to: " << current_page->sub_pages[current_index].page_display_name << std::endl;
        } else {
            std::cout << "Already at the first page in this level!" << std::endl;
        }
    }

    // Navigate to the next page (RIGHT)
    void navigateRight() {
        if (current_page->sub_pages.empty()) {
            std::cout << "No sub-pages at this level!" << std::endl;
            return;
        }

        if (current_index < current_page->sub_pages.size() - 1) {
            ++current_index;
            std::cout << "Navigated RIGHT to: " << current_page->sub_pages[current_index].page_display_name << std::endl;
        } else {
            std::cout << "Already at the last page in this level!" << std::endl;
        }
    }

    // Enter a sub-page (ENTER)
    void navigateEnter() {
        if (!current_page->sub_pages.empty()) {
            parent_pages.push_back(current_page);
            index_stack.push_back(current_index);

            current_page = &current_page->sub_pages[current_index];
            current_index = 0;

            std::cout << "Entered: " << current_page->page_display_name << std::endl;
        } else {
            std::cout << "No sub-pages to enter!" << std::endl;
        }
    }

    // Go back to the previous level (BACK)
    void navigateBack() {
        if (!parent_pages.empty()) {
            current_page = parent_pages.back();
            current_index = index_stack.back();

            parent_pages.pop_back();
            index_stack.pop_back();

            std::cout << "Navigated BACK to: " << current_page->page_display_name << std::endl;
        } else {
            std::cout << "Already at the root page!" << std::endl;
        }
    }

    // Display current page info
    void displayCurrentPage() const {
        std::cout << "Current Page: " << current_page->page_display_name
                  << " (" << current_page->page_name << ")" << std::endl;
    }

private:
    Page* current_page;                // Pointer to the current page
    size_t current_index;                 // Index in the current page's sub-pages

    std::vector<Page*> parent_pages;   // Stack to keep track of parent pages for BACK navigation
    std::vector<int> index_stack;      // Stack to keep track of the index at each level
};

// Recursive function to display the page hierarchy
void displayPages(const Page& page, int indent = 0) {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";  // Indentation for hierarchy
    }
    std::cout << "Page: " << page.page_display_name << " (" << page.page_name << ")\n";

    // Recursively display sub-pages
    for (const auto& sub_page : page.sub_pages) {
        displayPages(sub_page, indent + 1);
    }
}

int main() {
    // Creating the root page
    Page root("page_root", "Root", 1);

    // Creating the sub-pages of page_startup
    Page page_startup("page_startup", "Startup", 6);

    // Shape Selection Page and its sub-pages
    Page page_shape_selection("page_shape_selection", "Shape Selection", 5);
    page_shape_selection.addSubPage(Page("page_straight", "Straight", 1));
    page_shape_selection.addSubPage(Page("page_elbow_1", "Elbow 1", 1));
    page_shape_selection.addSubPage(Page("page_elbow_2", "Elbow 2", 1));
    page_shape_selection.addSubPage(Page("page_elbow_3", "Elbow 3", 1));
    page_shape_selection.addSubPage(Page("page_stairstep", "Stairstep", 1));

    page_startup.addSubPage(page_shape_selection);

    // Other sub-pages under page_startup
    page_startup.addSubPage(Page("page_camera_control", "Camera Control", 0));
    Page bend_rotate("page_bend_and_rotate", "Bend & Rotate", 1);
    bend_rotate.addSubPage(Page("page_bend_and_rotate_1", "Bend & Rotate 1", 1));
    bend_rotate.sub_pages[0].addSubPage(Page("page_bend_and_rotate_link_1", "Link 1", 0));
    page_startup.addSubPage(bend_rotate);

    Page joint_control("page_joint_control", "Joint Control", 1);
    joint_control.addSubPage(Page("page_joint_control_1", "Joint Control 1", 1));
    joint_control.sub_pages[0].addSubPage(Page("page_joint_control_link_1", "Link 1", 0));
    page_startup.addSubPage(joint_control);

    Page setup("page_setup", "Setup", 3);
    setup.addSubPage(Page("page_calibrate_joystick", "Calibrate Joystick", 1));
    setup.sub_pages[0].addSubPage(Page("page_calibrate_joystick_sub", "Calibrate Joystick", 0));
    setup.addSubPage(Page("page_pack_robot", "Pack Robot", 1));
    setup.sub_pages[1].addSubPage(Page("page_pack_robot_sub", "Pack Robot", 0));
    setup.addSubPage(Page("page_unpack_robot", "Unpack Robot", 1));
    setup.sub_pages[2].addSubPage(Page("page_unpack_robot_sub", "Unpack Robot", 0));
    page_startup.addSubPage(setup);

    Page information("page_information", "Information", 1);
    information.addSubPage(Page("page_information_sub", "Information", 0));
    page_startup.addSubPage(information);

    // Adding the startup page to the root
    root.addSubPage(page_startup);

    // Display the full hierarchy
    displayPages(root);

    // Create the PageNavigator and start navigating
    PageNavigator navigator(page_startup);

    // Sample navigation commands
    navigator.displayCurrentPage();
    navigator.navigateEnter();  // Enter the first sub-page (Startup)
    navigator.navigateRight();  // Move to the next page (Shape Selection)
    navigator.navigateEnter();  // Enter Shape Selection
    navigator.navigateRight();  // Move to the next sub-page (Elbow 1)
    navigator.navigateBack();   // Go back to the Startup page
    navigator.navigateRight();
    navigator.navigateRight();
    navigator.navigateRight();
    navigator.navigateRight();
    navigator.navigateRight();
    navigator.navigateRight();
    navigator.navigateLeft();
    navigator.navigateLeft();

    return 0;
}

#elif 1

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

// Recursive function to display the page hierarchy
void displayPages(const Page& page, int indent = 0) {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";  // Indentation for hierarchy
    }
    std::cout << "Page: " << page.page_display_name << " (" << page.page_name << ")\n";

    // Recursively display sub-pages
    for (const auto& sub_page : page.sub_pages) {
        displayPages(sub_page, indent + 1);
    }
}

int main() {
    // Creating the root page
    Page root("page_root", "Root", 1);

    // Creating the sub-pages of page_startup
    Page page_startup("page_startup", "Startup", 6);

    // Shape Selection Page and its sub-pages
    Page page_shape_selection("page_shape_selection", "Shape Selection", 5);
    page_shape_selection.addSubPage(Page("page_straight", "Straight", 1));
    page_shape_selection.addSubPage(Page("page_elbow_1", "Elbow 1", 1));
    page_shape_selection.addSubPage(Page("page_elbow_2", "Elbow 2", 1));
    page_shape_selection.addSubPage(Page("page_elbow_3", "Elbow 3", 1));
    page_shape_selection.addSubPage(Page("page_stairstep", "Stairstep", 1));

    page_startup.addSubPage(page_shape_selection);

    // Other sub-pages under page_startup
    page_startup.addSubPage(Page("page_camera_control", "Camera Control", 0));
    Page bend_rotate("page_bend_and_rotate", "Bend & Rotate", 1);
    bend_rotate.addSubPage(Page("page_bend_and_rotate_1", "Bend & Rotate 1", 1));
    bend_rotate.sub_pages[0].addSubPage(Page("page_bend_and_rotate_link_1", "Link 1", 0));
    page_startup.addSubPage(bend_rotate);

    Page joint_control("page_joint_control", "Joint Control", 1);
    joint_control.addSubPage(Page("page_joint_control_1", "Joint Control 1", 1));
    joint_control.sub_pages[0].addSubPage(Page("page_joint_control_link_1", "Link 1", 0));
    page_startup.addSubPage(joint_control);

    Page setup("page_setup", "Setup", 3);
    setup.addSubPage(Page("page_calibrate_joystick", "Calibrate Joystick", 1));
    setup.sub_pages[0].addSubPage(Page("page_calibrate_joystick_sub", "Calibrate Joystick", 0));
    setup.addSubPage(Page("page_pack_robot", "Pack Robot", 1));
    setup.sub_pages[1].addSubPage(Page("page_pack_robot_sub", "Pack Robot", 0));
    setup.addSubPage(Page("page_unpack_robot", "Unpack Robot", 1));
    setup.sub_pages[2].addSubPage(Page("page_unpack_robot_sub", "Unpack Robot", 0));
    page_startup.addSubPage(setup);

    Page information("page_information", "Information", 1);
    information.addSubPage(Page("page_information_sub", "Information", 0));
    page_startup.addSubPage(information);

    // Adding the startup page to the root
    root.addSubPage(page_startup);

    // Display the full hierarchy
    displayPages(root);

    return 0;
}
#endif
