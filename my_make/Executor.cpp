#include "Executor.h"
#include <iostream>
#include <vector>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sstream>
#include <stdexcept>

const int MAX_JOBS = 4; 

long long get_mtime(const std::string& path) {
    struct stat result;
    if (stat(path.c_str(), &result) == 0) {
        return (long long)result.st_mtime;
    }
    return -1;
}

void Executor::execute(std::unordered_map<std::string, Node>& nodes) {
    std::queue<std::string> ready_queue;
    std::unordered_map<pid_t, std::string> running_procs;
    int active_processes = 0;

    for (auto const& [name, node] : nodes) {
        if (node.in_degree == 0) {
            ready_queue.push(name);
        }
    }

    while (!ready_queue.empty() || active_processes > 0) {
        while (!ready_queue.empty() && active_processes < MAX_JOBS) {
            std::string current_file = ready_queue.front();
            ready_queue.pop();
            if (!nodes[current_file].has_command) {
                for (const std::string& dep : nodes[current_file].dependents) {
                    nodes[dep].in_degree--;
                    if (nodes[dep].in_degree == 0) ready_queue.push(dep);
                }
                continue;
            }

            long long target_time = get_mtime(current_file);
            bool needs_update = (target_time == -1);

            if (!needs_update) {
                for (const std::string& dep_name : nodes[current_file].dependencies) {
                    if (get_mtime(dep_name) > target_time) {
                        needs_update = true;
                        break;
                    }
                }
            }

            if (!needs_update) {
                std::cout << "[Skip] '" << current_file << "' is up to date." << std::endl;
                for (const std::string& dep : nodes[current_file].dependents) {
                    nodes[dep].in_degree--;
                    if (nodes[dep].in_degree == 0) ready_queue.push(dep);
                }
                continue;
            }
            pid_t pid = fork();
            if (pid == 0) {
                run_command(nodes[current_file].command);
                exit(0); 
            } else if (pid > 0) {
                running_procs[pid] = current_file;
                active_processes++;
                std::cout << "[Started] " << current_file << " (Parallel Jobs: " << active_processes << ")" << std::endl;
            } else {
                throw std::runtime_error("Failed to fork process for " + current_file);
            }
        }
        if (active_processes > 0) {
            int status;
            pid_t finished_pid = wait(&status); 
            
            if (finished_pid > 0) {
                std::string finished_file = running_procs[finished_pid];
                active_processes--;
                if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                    std::cout << "[Finished] " << finished_file << std::endl;
                    for (const std::string& dep : nodes[finished_file].dependents) {
                        nodes[dep].in_degree--;
                        if (nodes[dep].in_degree == 0) ready_queue.push(dep);
                    }
                } else {
                    throw std::runtime_error("Build failed: Command for '" + finished_file + "' exited with error.");
                }
            }
        }
    }

    check_for_cycles(nodes);
}

void Executor::run_command(const std::string& command) {
    std::vector<char*> args;
    std::stringstream ss(command);
    std::string arg;
    
    while (ss >> arg) {
        char* c_arg = new char[arg.size() + 1];
        std::copy(arg.begin(), arg.end(), c_arg);
        c_arg[arg.size()] = '\0';
        args.push_back(c_arg);
    }
    args.push_back(nullptr);

    execvp(args[0], args.data());
    perror("execvp failed");
    exit(1);
}

void Executor::check_for_cycles(const std::unordered_map<std::string, Node>& nodes) {
    for (auto const& [name, node] : nodes) {
        if (node.in_degree > 0) {
            throw std::runtime_error("Error: Dependency cycle or unresolved target for '" + name + "'.");
        }
    }
}
