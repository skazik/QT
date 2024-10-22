#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

#include <yaml-cpp/yaml.h>
#include <nlohmann/json.hpp> // For JSON handling
using json = nlohmann::json;


#include "page_tree.h"
#include "navigator.hpp"

std::string trim(const std::string& str);
std::string trimLeadingWhitespaceFromEachLine(const std::string& str);

void copy_from_file(const std::string& src);
void copy_to_file(const std::string& dest);

void traverse_pagetree_recursive_json(PageNode* currentNode, json& output_json, const char *root_name, bool enter_on_right = false);
void traverse_pagetree_recursive_yaml(PageNode* currentNode, YAML::Node& output_yaml, const char *root_name, bool enter_on_right = false);
void traverse_pagetree_json(PageNode *root, const char* root_name);
void traverse_pagetree_yaml(PageNode *root, const char* root_name);

#endif // UTILS_H
