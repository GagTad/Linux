#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <unordered_map>
#include <string>
#include <queue>
#include <sys/types.h>
#include "Node.h"

class Executor {
public:
    static void execute(std::unordered_map<std::string, Node>& nodes);
private:
    static void run_command(const std::string& command);
    static void check_for_cycles(const std::unordered_map<std::string, Node>& nodes);
};

#endif
