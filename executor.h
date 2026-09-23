#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <string>
#include <vector>

// Searches PATH for the given command name.
// Returns full path to the executable if found and accessible, or empty string.
std::string find_path(const std::string& cmd);

// Executes an external program with redirection support using fork/execv.
void execute_external(const std::vector<std::string>& args);

#endif // EXECUTOR_H
