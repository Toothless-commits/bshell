#include "autocomplete.h"
#include <sstream>
#include <filesystem>
#include <dirent.h>
#include <cstdlib>

namespace fs = std::filesystem;

Autocompleter::Autocompleter() = default;

void Autocompleter::register_builtin(const std::string& name) {
    root.insert(name);
}

void Autocompleter::load_path_commands() {
    const char* path_env = std::getenv("PATH");
    if (!path_env) return;

    std::stringstream ss(path_env);
    std::string dir;

    while (std::getline(ss, dir, ':')) {
        DIR* di = opendir(dir.c_str());
        if (!di) continue;

        struct dirent* entry;
        while ((entry = readdir(di)) != nullptr) {
            std::string name = entry->d_name;
            if (name == "." || name == "..") continue;
            commands.insert(name);
        }
        closedir(di);
    }
}

void Autocompleter::load_files() {
    files.clear();
    std::string path = fs::current_path().string();
    DIR* di = opendir(path.c_str());
    if (!di) return;

    struct dirent* entry;
    while ((entry = readdir(di)) != nullptr) {
        std::string name = entry->d_name;
        if (name == "." || name == "..") continue;
        files.insert(name);
    }
    closedir(di);
}

std::vector<std::string> Autocompleter::match(const std::string& line) const {
    std::vector<std::string> m;
    auto it = commands.lower_bound(line);

    while (it != commands.end() && it->starts_with(line)) {
        m.push_back(*it);
        it++;
    }

    return m;
}

std::string Autocompleter::longest_common_prefix(const std::vector<std::string>& matches) const {
    if (matches.empty()) return "";
    std::string lcp = matches[0];
    for (const auto& it : matches) {
        size_t i = 0;
        while (i < lcp.size() && i < it.size() && lcp[i] == it[i]) {
            i++;
        }
        lcp = lcp.substr(0, i);
    }
    return lcp;
}

std::string Autocompleter::file_autocomplete(const std::string& name) const {
    std::string m = "";
    auto it = files.lower_bound(name);
    if (it != files.end() && it->starts_with(name)) {
        m = name;
    }
    return m;
}

std::string Autocompleter::trie_autocomplete(const std::string& cur) {
    return root.autocomplete(cur);
}

bool Autocompleter::has_files() const {
    return !files.empty();
}
