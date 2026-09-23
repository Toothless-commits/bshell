#include "executor.h"
#include "parser.h"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

std::string find_path(const std::string& cmd) {
    const char* path = std::getenv("PATH");
    if (!path) {
        return "";
    }

    std::stringstream ss(path);
    std::string dir;

    while (std::getline(ss, dir, ':')) {
        std::string candidate = dir + "/" + cmd;
        if (access(candidate.c_str(), X_OK) == 0) {
            return candidate;
        }
    }

    return "";
}

void execute_external(const std::vector<std::string>& ar) {
    if (ar.empty()) return;

    std::string path = find_path(ar[0]);

    if (!path.empty()) {
        int out = find_redirection(ar);
        std::vector<char*> argv;

        if (out == -1) {
            for (const auto& s : ar) {
                argv.push_back(const_cast<char*>(s.data()));
            }
        } else {
            for (int i = 0; i < out; i++) {
                argv.push_back(const_cast<char*>(ar[i].data()));
            }
        }
        argv.push_back(nullptr);

        pid_t pid = fork();

        if (pid == 0) {
            int fd = -1;
            if (out != -1) {
                const std::string& op = ar[out];
                const std::string& filename = ar[out + 1];

                if (op == ">" || op == "1>" || op == "2>") {
                    fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
                } else if (op == ">>" || op == "1>>" || op == "2>>") {
                    fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
                }

                if (fd == -1) {
                    perror("open");
                    exit(1);
                }

                if (op == "2>" || op == "2>>") {
                    dup2(fd, 2);
                } else {
                    dup2(fd, 1);
                }

                close(fd);
            }

            execv(path.c_str(), argv.data());
            perror("execv");
            exit(1);
        }

        wait(nullptr);
        std::cout << std::flush;
    } else {
        std::cout << ar[0] << ": command not found\n";
    }
}
