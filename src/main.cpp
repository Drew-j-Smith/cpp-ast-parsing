
#include "expression.h"
#include "parser.h"

int main() {
    std::map<std::string_view, double> variables{{"var1", 1}, {"var2", 2}, {"var3", 3}, {"var4", 4}, {"var5", 5}, {"var6", 6}};

    std::string s;
    try {
        while (true) {
            std::cout << "Enter expression\n";
            std::getline(std::cin, s);
            auto p = parse_expression(s);
            if (std::holds_alternative<AddExpression>(p)) {
                std::cout << std::get<AddExpression>(p).evaluate(variables) << '\n';
            } else if (std::holds_alternative<MultExpression>(p)) {
                std::cout << std::get<MultExpression>(p).evaluate(variables) << '\n';
            } else {
                std::cerr << "Unknown value\n";
                break;
            }
        }
    }
    catch (std::exception& e) {
        std::cout << e.what();
    }
}