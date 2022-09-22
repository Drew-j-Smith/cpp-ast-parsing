

#include "parser/lexer.h"
#include <iostream>

struct identifier : public token {
    constexpr static ctll::fixed_string capture_name = "identifier";
    constexpr static std::string_view regex =
        "\\s*(?<identifier>[a-zA-Z][a-zA-Z0-9_]*)\\s*";
};
struct number : public token {
    constexpr static ctll::fixed_string capture_name = "number";
    constexpr static std::string_view regex = "\\s*(?<number>[0-9]+)\\s*";
};

int main() {
    std::string_view str = "test\t\n test 89";
    const char *end = str.data();
    for (const auto token : lexer<identifier, number>{str}) {
        std::visit(
            [&](auto &&arg) {
                std::cout << arg.str << '.';
                end = arg.str.data() + arg.str.size();
            },
            token);
    }
    if (end != str.data() + str.size()) {
        std::cout << "incomplete parse";
    }
}