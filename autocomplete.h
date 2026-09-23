#ifndef AUTOCOMPLETE_H
#define AUTOCOMPLETE_H

#include <string>
#include <vector>
#include <set>
#include "trie.h"

class Autocompleter {
public:
    Autocompleter();

    void register_builtin(const std::string& name);
    void load_path_commands();
    void load_files();

    std::vector<std::string> match(const std::string& line) const;
    std::string longest_common_prefix(const std::vector<std::string>& matches) const;
    std::string file_autocomplete(const std::string& name) const;
    std::string trie_autocomplete(const std::string& cur);

    bool has_files() const;

private:
    Trie root;
    std::set<std::string> commands;
    std::set<std::string> files;
};

#endif // AUTOCOMPLETE_H
