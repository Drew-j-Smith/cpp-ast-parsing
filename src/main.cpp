
#include "expression.h"
#include "parser.h"

int main() {
    auto p = parse_expression("var1+var2*var3+((var4+var5)*var6)");
    std::visit([](auto &variant) { std::cout << variant << '\n'; }, p);
    std::cout << "success!\n";
}