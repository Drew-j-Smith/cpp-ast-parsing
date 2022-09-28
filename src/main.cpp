
#include "parser/lookahead.h"
#include "script/script.h"
#include <string>

int main() {
    std::map<std::string, int> variables{};

    std::string s;
    while (true) {
        try {
            std::cout << ">>> ";
            std::getline(std::cin, s);
            if (s.length() == 0) {
                continue;
            }
            auto p = parse_expression(s);
            std::visit(
                Overload{[&](const AddExpression &a) {
                             std::cout << a.evaluate(variables) << '\n';
                         },
                         [&](const Assignment &a) {
                             int value = a.a.evaluate(variables);
                             variables[std::string{a.i.str}] = value;
                         },
                         [&](const IfExpression &i) {
                             if (auto value = i.evaluate(variables)) {
                                 std::cout << value.value() << '\n';
                             }
                         },
                         [](const ParseError &e) { std::cout << e << '\n'; },
                         [](const TokenizeError &e) { std::cout << e << '\n'; },
                         [](const auto &o) {
                             std::cerr << "Unknown type: "
                                       << typeid(decltype(o)).name() << '\n';
                         }},
                p);
        } catch (std::exception &e) {
            std::cout << e.what() << '\n';
        }
    }
}