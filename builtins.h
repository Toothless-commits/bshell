#ifndef BUILTINS_H
#define BUILTINS_H

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>

class BuiltinManager {
public:
    enum Status {
        STATUS_OK = 0,
        STATUS_EXIT = 1,
        STATUS_ERROR = 2
    };

    using BuiltinHandler = std::function<int(const std::string& line, const std::vector<std::string>& args, BuiltinManager& mgr)>;

    BuiltinManager();

    bool is_builtin(const std::string& name) const;
    int execute(const std::string& name, const std::string& line, const std::vector<std::string>& args);
    const std::unordered_set<std::string>& get_builtin_names() const;
    void register_builtin(const std::string& name, BuiltinHandler handler);

private:
    std::unordered_map<std::string, BuiltinHandler> handlers;
    std::unordered_set<std::string> names;
};

#endif // BUILTINS_H
