
#include "parser.h"
#include "proposition.h"
#include <string>

std::ostream &operator<<(std::ostream &out, std::monostate) {
    return out << "monostate";
}

int main() {

    constexpr Parser<WhiteSpace, AndExpression, OrExpression, Identifier,
                     NotSymbol, InvertableIdentifier, AndSymbol, OrSymbol>
        p{};
    while (true) {
        std::string input;
        std::getline(std::cin, input);
        if (input.size() == 0) {
            continue;
        }
        auto res = p.parse(input);
        std::visit([](auto &&arg) { std::cout << arg << '\n'; }, res);
    }
}