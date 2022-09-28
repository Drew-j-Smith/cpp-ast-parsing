
#include <string>
#include <variant>
#include <vector>

struct Variable {
    std::variant<Variable (*)(Variable), int, std::string,
                 std::vector<Variable>>
        data;
};