#include <iostream>
#include <unordered_map>
#include "Node.h"
#include "Parser.h"
#include "Executor.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <build_file>" << std::endl;
        return 1;
    }
    std::string build_file = argv[1];
    try {
        std::cout << "--- Parsing build file: " << build_file << " ---" << std::endl;
        auto nodes = Parser::parse(build_file);

        if (nodes.empty()) {
            std::cout << "Nothing to build." << std::endl;
            return 0;
        }
        std::cout << "--- Starting parallel build ---" << std::endl;
        Executor::execute(nodes);
        std::cout << "--- Build successful ---" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Build stopped: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
