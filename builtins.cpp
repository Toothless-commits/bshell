#include "builtins.h"
#include "parser.h"
#include "executor.h"
#include <iostream>
#include <sstream>
#include <filesystem>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cstdlib>

namespace fs = std::filesystem;

namespace {

int builtin_exit(const std::string& /*line*/, const std::vector<std::string>& /*args*/, BuiltinManager& /*mgr*/) {
    return BuiltinManager::STATUS_EXIT;
}

int builtin_echo(const std::string& line, const std::vector<std::string>& /*args*/, BuiltinManager& /*mgr*/) {
    std::vector<std::string> ar = tokenize(line);
    int out = find_redirection(ar);

    if (out == -1) {
        for (size_t i = 1; i < ar.size(); i++) {
            std::cout << ar[i] << " ";
        }
        std::cout << std::endl;
    } else {
        int fd = -1;
        const std::string& op = ar[out];
        const std::string& filename = ar[out + 1];

        if (op == ">" || op == "1>" || op == "2>") {
            fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        } else if (op == ">>" || op == "1>>" || op == "2>>") {
            fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
        }

        if (op == ">" || op == "1>" || op == ">>" || op == "1>>") {
            for (int i = 1; i < out; i++) {
                write(fd, ar[i].c_str(), std::strlen(ar[i].data()));
                write(fd, " ", 1);
            }
            write(fd, "\n", 1);
            close(fd);
        } else {
            dup2(fd, STDERR_FILENO);
            close(fd);
            for (int i = 1; i < out; i++) {
                std::cout << ar[i] << " ";
            }
            std::cout << std::endl;
        }
    }

    return BuiltinManager::STATUS_OK;
}

int builtin_pwd(const std::string& /*line*/, const std::vector<std::string>& /*args*/, BuiltinManager& /*mgr*/) {
    std::cout << fs::current_path().string() << '\n';
    return BuiltinManager::STATUS_OK;
}

int builtin_type(const std::string& line, const std::vector<std::string>& /*args*/, BuiltinManager& mgr) {
    std::stringstream ss(line);
    std::string first_token, cmd;
    ss >> first_token;
    ss >> cmd;

    if (mgr.is_builtin(cmd)) {
        std::cout << cmd << " is a shell builtin\n";
    } else {
        std::string path = find_path(cmd);
        if (!path.empty()) {
            std::cout << cmd << " is " << path << '\n';
        } else {
            std::cout << cmd << ": not found\n";
        }
    }

    return BuiltinManager::STATUS_OK;
}

int builtin_cd(const std::string& line, const std::vector<std::string>& /*args*/, BuiltinManager& /*mgr*/) {
    std::stringstream ss(line);
    std::string first_token, path;
    ss >> first_token;
    ss >> path;

    if (path == "~") {
        const char* home = std::getenv("HOME");
        if (home) {
            path = home;
        }
    }

    if (chdir(path.c_str()) != 0) {
        std::cout << "cd: " << path << ": No such file or directory" << std::endl;
    }

    return BuiltinManager::STATUS_OK;
}

} // anonymous namespace

BuiltinManager::BuiltinManager() {
    register_builtin("exit", builtin_exit);
    register_builtin("echo", builtin_echo);
    register_builtin("pwd", builtin_pwd);
    register_builtin("type", builtin_type);
    register_builtin("cd", builtin_cd);
}

void BuiltinManager::register_builtin(const std::string& name, BuiltinHandler handler) {
    handlers[name] = std::move(handler);
    names.insert(name);
}

bool BuiltinManager::is_builtin(const std::string& name) const {
    return handlers.find(name) != handlers.end();
}

int BuiltinManager::execute(const std::string& name, const std::string& line, const std::vector<std::string>& args) {
    auto it = handlers.find(name);
    if (it != handlers.end()) {
        return it->second(line, args, *this);
    }
    return STATUS_ERROR;
}

const std::unordered_set<std::string>& BuiltinManager::get_builtin_names() const {
    return names;
}
