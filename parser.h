#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>

// Tokenizes input line into arguments, supporting single quotes, double quotes,
// backslash escapes, and whitespace separation.
std::vector<std::string> tokenize(const std::string& line);

// Identifies the index of redirection operators (>, 1>, 2>, >>, 1>>, 2>>).
// Returns -1 if no redirection operator is found.
int find_redirection(const std::vector<std::string>& args);

#endif // PARSER_H
