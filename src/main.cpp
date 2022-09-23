
#include "parser/lookahead.h"
#include "script/script.h"
#include <string>

int main() {
    std::map<std::string, double> variables{};

    std::string s;
    while (true) {
        std::cout << "Enter expression\n";
        std::getline(std::cin, s);
        auto p = parse_expression(s);
        std::visit(
            Overload{[&](const AddExpression &a) {
                         std::cout << a.evaluate(variables) << '\n';
                     },
                     [&](const Assignment &a) {
                         double value = a.a->evaluate(variables);
                         variables[std::string{a.i.str}] = value;
                     },
                     [](const ParseError &e) { std::cout << e << '\n'; },
                     [](const TokenizeError &e) { std::cout << e << '\n'; },
                     [](const auto &o) {
                         std::cerr
                             << "Unknown type: " << typeid(decltype(o)).name()
                             << '\n';
                     }},
            p);
    }
}