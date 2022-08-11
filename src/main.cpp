
// #include "parser.h"
// #include "proposition.h"
#include "parser2.h"
#include <string>

std::ostream &operator<<(std::ostream &out, std::monostate) {
    return out << "monostate";
}

void v1() {
    // constexpr Parser<WhiteSpace, AndExpression, OrExpression, Identifier,
    //                  NotSymbol, InvertableIdentifier, AndSymbol, OrSymbol>
    //     p{};
    // while (true) {
    //     std::string input;
    //     std::getline(std::cin, input);
    //     if (input.size() == 0) {
    //         continue;
    //     }
    //     auto res = p.parse(input);
    //     std::visit([](auto &&arg) { std::cout << arg << '\n'; }, res);
    // }
}

void v2() {
    auto p = parse(
        Terminals<Identifier, TermialCharacter<'+'>, TermialCharacter<'*'>>{},
        Symbols<MultExpression, AddExpression>{}, "var1+var2*var3+var4");
    auto add = std::move(std::get<AddExpression>(p));
    std::cout << add << '\n';
    std::cout << "success!\n";
}

int main() { v2(); }