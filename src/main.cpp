
#include "parser/lookahead.h"
#include "script/script.h"
#include <cstdlib>
#include <string>

std::map<std::string, Variable> variables{};

int main() {
    variables["print"] = Variable{[](const std::vector<Variable> &to_print) {
        for (const auto &var : to_print) {
            std::visit(
                Overload{[](const auto &val) { std::cout << val << '\n'; },
                         [](std::vector<Variable>) { std::cout << "vector\n"; },
                         [](Variable (*)(const std::vector<Variable> &)) {
                             std::cout << "function\n";
                         }},
                var.data);
        }
        return Variable{0};
    }};
}

extern "C" {
void wasm_parse(char *input) {
    try {
        auto p = parse_expression(std::string_view{input, strlen(input)});
        std::visit(
            Overload{[&](const Block &b) { b.evaluate(variables); },
                     [](const ParseError &e) { std::cout << e << '\n'; },
                     [](const TokenizeError &e) { std::cout << e << '\n'; },
                     [](const auto &o) {
                         std::cerr
                             << "Unknown type: " << typeid(decltype(o)).name()
                             << '\n';
                     }},
            p);
    } catch (std::exception &e) {
        std::cout << e.what() << '\n';
    }
}
}