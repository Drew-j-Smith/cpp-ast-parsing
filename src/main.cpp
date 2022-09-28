
#include "parser/lookahead.h"
#include "script/script.h"
#include <string>

Variable print(Variable to_print) {
    std::visit(
        Overload{[](const auto &val) { std::cout << val << '\n'; },
                 [](std::vector<Variable>) { std::cout << "vector\n"; },
                 [](Variable (*)(Variable)) { std::cout << "function\n"; }},
        to_print.data);
    return Variable{0};
}

int main() {
    std::map<std::string, Variable> variables{};

    variables["print"] = Variable{print};

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
                             std::get<int>(a.evaluate(variables).data);
                         },
                         [&](const Assignment &a) {
                             auto value = a.a.evaluate(variables);
                             variables[std::string{a.i.str}] = value;
                         },
                         [&](const IfExpression &i) {
                             if (auto value = i.evaluate(variables)) {
                                 std::cout << std::get<int>(value.value().data)
                                           << '\n';
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