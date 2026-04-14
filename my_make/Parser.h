#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <unordered_map>
#include "Node.h"

class Parser {
public:
    static std::unordered_map<std::string, Node> parse(const std::string& filename);
private:
    static std::string trim(const std::string& s);
};

#endif
