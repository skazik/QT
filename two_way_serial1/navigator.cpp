#include "navigator.hpp"

// Creating the root page
static Page root("page_root", "Root", 1);

// Creating the sub-pages of page_startup
static Page page_startup("page_startup", "Startup", 6);

// Shape Selection Page and its sub-pages
static Page page_shape_selection("page_shape_selection", "Shape Selection", 5);

static Page bend_rotate("page_bend_and_rotate", "Bend & Rotate", 1);
static Page joint_control("page_joint_control", "Joint Control", 1);
static Page setup("page_setup", "Setup", 3);
static Page information("page_information", "Information", 1);

// Create the PageNavigator and start navigating
static PageNavigator navigator(page_startup);

PageNavigator* navigator_get() {
    return &navigator;
}

// Recursive function to display the page hierarchy
void displayPages(const Page& page, int indent) {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";  // Indentation for hierarchy
    }
    std::cout << "Page: " << page.page_display_name << " (" << page.page_name << ")\n";

    // Recursively display sub-pages
    for (const auto& sub_page : page.sub_pages) {
        displayPages(sub_page, indent + 1);
    }
}

void navigator_init() {
    // Shape Selection Page and its sub-pages
    page_shape_selection.addSubPage(Page("page_straight", "Straight", 1));
    page_shape_selection.addSubPage(Page("page_elbow_1", "Elbow 1", 1));
    page_shape_selection.addSubPage(Page("page_elbow_2", "Elbow 2", 1));
    page_shape_selection.addSubPage(Page("page_elbow_3", "Elbow 3", 1));
    page_shape_selection.addSubPage(Page("page_stairstep", "Stairstep", 1));

    page_startup.addSubPage(page_shape_selection);

    // Other sub-pages under page_startup
    page_startup.addSubPage(Page("page_camera_control", "Camera Control", 0));
    bend_rotate.addSubPage(Page("page_bend_and_rotate_1", "Bend & Rotate 1", 1));
    bend_rotate.sub_pages[0].addSubPage(Page("page_bend_and_rotate_link_1", "Link 1", 0));
    page_startup.addSubPage(bend_rotate);

    joint_control.addSubPage(Page("page_joint_control_1", "Joint Control 1", 1));
    joint_control.sub_pages[0].addSubPage(Page("page_joint_control_link_1", "Link 1", 0));
    page_startup.addSubPage(joint_control);

    setup.addSubPage(Page("page_calibrate_joystick", "Calibrate Joystick", 1));
    setup.sub_pages[0].addSubPage(Page("page_calibrate_joystick_sub", "Calibrate Joystick", 0));
    setup.addSubPage(Page("page_pack_robot", "Pack Robot", 1));
    setup.sub_pages[1].addSubPage(Page("page_pack_robot_sub", "Pack Robot", 0));
    setup.addSubPage(Page("page_unpack_robot", "Unpack Robot", 1));
    setup.sub_pages[2].addSubPage(Page("page_unpack_robot_sub", "Unpack Robot", 0));
    page_startup.addSubPage(setup);

    information.addSubPage(Page("page_information_sub", "Information", 0));
    page_startup.addSubPage(information);

    // Adding the startup page to the root
    root.addSubPage(page_startup);

}
