
#include <string>
#include <variant>
#include <vector>

struct Variable {
    std::variant<Variable (*)(const std::vector<Variable> &), int, std::string,
                 std::vector<Variable>>
        data;
};