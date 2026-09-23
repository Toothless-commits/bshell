#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "terminal.h"
#include "autocomplete.h"
#include "line_editor.h"
#include "parser.h"
#include "builtins.h"
#include "executor.h"

int main() {
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    Terminal term;
    term.enableDirectInput();

    Autocompleter completer;
    BuiltinManager builtins;

    for (const auto& name : builtins.get_builtin_names()) {
        completer.register_builtin(name);
    }
    completer.load_path_commands();

    LineEditor editor(term, completer);

    std::string line;
    while (editor.read_line(line)) {
        std::stringstream ss(line);
        std::string arg;
        ss >> arg;

        if (arg.empty()) {
            continue;
        }

        if (builtins.is_builtin(arg)) {
            int status = builtins.execute(arg, line, tokenize(line));
            if (status == BuiltinManager::STATUS_EXIT) {
                break;
            }
        } else {
            std::vector<std::string> ar = tokenize(line);
            execute_external(ar);
        }
    }

    term.normalmode();
    return 0;
}
