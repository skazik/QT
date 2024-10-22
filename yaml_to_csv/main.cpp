#include <iostream>
#include <fstream>
#include <yaml-cpp/yaml.h>
#include <vector>
#include <string>

// Helper function to write a line to CSV with leading commas based on the level
void writeCSVLine(std::ofstream& csvFile, const std::string& pageName, int level) {
    if (csvFile.is_open()) {
        for (int i = 0; i < level; ++i) {
            csvFile << ",";
        }
        csvFile << pageName << std::endl;
    }
}

void yamlToCSV(const YAML::Node& node, std::ofstream& csvFile, int level = 0) {
    if (node["page_display_name"] && node["page_display_name"].IsScalar()) {
        std::string pageName = node["page_display_name"].as<std::string>();
        if (!pageName.empty()) {
            std::cout << "Writing: " << pageName << " at level " << level << std::endl;
            writeCSVLine(csvFile, pageName, level);
        } else {
            std::cerr << "Warning: Empty 'page_display_name' at level " << level << std::endl;
        }
    } else {
        std::cerr << "Warning: 'page_display_name' is missing or not a scalar value at level " << level << std::endl;
    }

    // Recursively process sub-pages if they exist
    if (node["sub_pages"] && node["sub_pages"].IsSequence()) {
        for (const auto& subPage : node["sub_pages"]) {
            yamlToCSV(subPage, csvFile, level + 1);
        }
    } else if (node["sub_pages"]) {
        std::cerr << "Warning: 'sub_pages' is not a sequence at level " << level << std::endl;
    }
}


// Function to change the file extension from .yaml to .csv
std::string changeExtension(const std::string& filename, const std::string& newExtension) {
    size_t lastDot = filename.find_last_of(".");
    if (lastDot == std::string::npos) return filename + newExtension;
    return filename.substr(0, lastDot) + newExtension;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input.yaml>" << std::endl;
        return 1;
    }

    std::string inputFile = argv[1];
    std::string outputFile = changeExtension(inputFile, ".csv");

    std::cout << "Loading YAML file: " << inputFile << std::endl;
    YAML::Node yamlData = YAML::LoadFile(inputFile);

    if (!yamlData) {
        std::cerr << "Error: YAML file is empty or not loaded correctly." << std::endl;
        return 1;
    }

    std::cout << "YAML file loaded successfully." << std::endl;

    // Check for the presence of the "pages" key
    if (!yamlData["pages"] || !yamlData["pages"].IsSequence()) {
        std::cerr << "Error: The 'pages' key is missing or is not a sequence in the YAML file." << std::endl;
        return 1;
    }

    // Open the output CSV file
    std::ofstream csvFile(outputFile);
    if (!csvFile.is_open()) {
        std::cerr << "Error: Unable to open output file " << outputFile << std::endl;
        return 1;
    }

    // Convert the YAML to CSV by iterating over each page in "pages"
    for (const auto& page : yamlData["pages"]) {
        yamlToCSV(page, csvFile);
    }

    std::cout << "Conversion complete: " << outputFile << std::endl;
    return 0;
}
