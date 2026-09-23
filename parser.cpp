#include "parser.h"
#include <cctype>

std::vector<std::string> tokenize(const std::string& line) {
    std::vector<std::string> ans;
    int n = line.size();
    bool inquotes = false;
    bool doublequot = false;
    std::string current = "";
    bool backslash = false;
    bool inbackslashquote = false;

    for (int i = 0; i < n; i++) {
        char ch = line[i];

        if (backslash) {
            current += ch;
            backslash = false;
        } else if (doublequot) {
            if (ch == '\"' && !inbackslashquote) {
                doublequot = false;
            } else if (ch == '\\' || inbackslashquote) {
                if (ch == '\\' && !inbackslashquote) {
                    inbackslashquote = true;
                } else if (inbackslashquote) {
                    current += ch;
                    inbackslashquote = false;
                }
            } else {
                current += ch;
            }
        } else if (inquotes) {
            if (ch == '\'') {
                inquotes = false;
            } else {
                current += ch;
            }
        } else {
            if (ch == '\\') {
                backslash = true;
            } else if (ch == '\"') {
                doublequot = true;
            } else if (ch == '\'') {
                inquotes = true;
            } else if (std::isspace(static_cast<unsigned char>(ch))) {
                if (!current.empty()) {
                    ans.emplace_back(current);
                    current.clear();
                }
            } else {
                current += ch;
            }
        }
    }

    if (!current.empty()) {
        ans.emplace_back(current);
    }

    return ans;
}

int find_redirection(const std::vector<std::string>& ar) {
    for (size_t i = 0; i < ar.size(); i++) {
        if (ar[i] == ">" || ar[i] == "1>" || ar[i] == "2>" ||
            ar[i] == ">>" || ar[i] == "1>>" || ar[i] == "2>>") {
            return static_cast<int>(i);
        }
    }
    return -1;
}
