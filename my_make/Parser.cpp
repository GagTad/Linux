#include "Parser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

std::unordered_map<std::string, Node> Parser::parse(const std::string& filename) {
    std::unordered_map<std::string, Node> nodes;
    std::ifstream file(filename);
    std::string line;
    int line_number = 0;

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    while (std::getline(file, line)) {
        line_number++;
        line = trim(line);
        if (line.empty()) continue; 

        size_t colon_pos = line.find(':');
        if (colon_pos != std::string::npos) {
            std::string target_name = trim(line.substr(0, colon_pos));
          
            if (target_name.empty()) {
                throw std::runtime_error("Error at line " + std::to_string(line_number) + ": Target name is missing before ':'");
            }

            std::string deps_part = line.substr(colon_pos + 1);
            if (nodes.find(target_name) == nodes.end()) {
                nodes[target_name] = Node(target_name);
            }

            std::stringstream ss(deps_part);
            std::string dep_name;
            while (ss >> dep_name) {
                nodes[target_name].in_degree++;
                nodes[target_name].dependencies.push_back(dep_name);
                if (nodes.find(dep_name) == nodes.end()) {
                    nodes[dep_name] = Node(dep_name);
                }
                nodes[dep_name].dependents.push_back(target_name);
            }
            std::string command_line;
            if (std::getline(file, command_line)) {
                line_number++;
                command_line = trim(command_line);
                if (command_line.empty()) {
                     std::cerr << "Warning at line " << line_number << ": Target '" << target_name << "' has an empty command." << std::endl;
                } else {
                    nodes[target_name].command = command_line;
                    nodes[target_name].has_command = true;
                }
            } else {
                throw std::runtime_error("Error: Unexpected EOF. Target '" + target_name + "' expects a command line.");
            }
        } else {
            throw std::runtime_error("Error at line " + std::to_string(line_number) + ": Invalid format (missing ':').");
        }
    }
    return nodes;
}

std::string Parser::trim(const std::string& s) {
    size_t first = s.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = s.find_last_not_of(" \t\r\n");
    return s.substr(first, (last - first + 1));
}
