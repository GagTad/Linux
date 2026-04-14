#ifndef NODE_H
#define NODE_H

#include <string>
#include <vector>

struct Node {
    std::string name;             
    std::string command;         
    int in_degree = 0;           
    std::vector<std::string> dependents;
    std::vector<std::string> dependencies; 
    bool has_command = false;
    Node(std::string n = "") : name(n), in_degree(0), has_command(false) {}
};

#endif
