
#include <charconv>
#include <iostream>
#include <memory>
#include <optional>
#include <string_view>
#include <system_error>
#include <vector>

struct Symbol {
    virtual ~Symbol() = default;
};

class Parser {
public:
    using TerminalFp =
        std::optional<std::unique_ptr<Symbol>> (*)(std::string_view &);
    using RuleFp = bool (*)(std::vector<std::unique_ptr<Symbol>> &);

private:
    std::vector<TerminalFp> terminalSymbols;
    std::vector<RuleFp> rules;

    bool reduce(std::vector<std::unique_ptr<Symbol>> &parseStack) {
        for (auto rule : rules) {
            if (rule(parseStack)) {
                return true;
            }
        }
        return false;
    }

    bool shift(std::vector<std::unique_ptr<Symbol>> &parseStack,
               std::string_view &str) {
        for (auto terminal : terminalSymbols) {
            if (auto symbol = terminal(str)) {
                parseStack.push_back(std::move(symbol.value()));
                return true;
            }
        }
        return false;
    }

public:
    Parser(std::vector<TerminalFp> terminalSymbols, std::vector<RuleFp> rules)
        : terminalSymbols{std::move(terminalSymbols)}, rules{std::move(rules)} {
    }

    std::unique_ptr<Symbol> parse(std::string_view str) {
        std::vector<std::unique_ptr<Symbol>> parseStack;
        while (str.size() > 0) {
            if (!shift(parseStack, str)) {
                std::cout << "Error parsing\n";
                return nullptr;
            }
            while (reduce(parseStack))
                ;
        }
        if (parseStack.size() > 1) {
            std::cout << "incomplete parse\n";
        }
        return std::move(parseStack[0]);
    }
};

struct IntSymbol final : public Symbol {
    int val;
    IntSymbol(int val) : val{val} {}

    static std::optional<std::unique_ptr<Symbol>> shift(std::string_view &str) {
        int result{};

        auto [ptr,
              ec]{std::from_chars(str.data(), str.data() + str.size(), result)};

        if (ec == std::errc{}) {
            str = std::string_view(ptr, str.data() + str.size() - ptr);
            return std::make_unique<IntSymbol>(result);
        } else if (ec == std::errc::invalid_argument) {
            return {};
        } else if (ec == std::errc::result_out_of_range) {
            std::cout << "Larger than an int.\n";
            return {};
        } else {
            return {};
        }
    }
};

class AddSymbol final : public Symbol {
public:
    static std::optional<std::unique_ptr<Symbol>> shift(std::string_view &str) {
        if (str[0] == '+') {
            str = std::string_view(str.data() + 1, str.size() - 1);
            return std::make_unique<AddSymbol>();
        }
        return {};
    }
};

struct AddExpression final : public Symbol {
    IntSymbol left;
    AddSymbol add;
    IntSymbol right;

    AddExpression(IntSymbol left, AddSymbol add, IntSymbol right)
        : left(left), add(add), right(right) {}
};

bool createAddExpression(std::vector<std::unique_ptr<Symbol>> &vec) {
    if (vec.size() != 3) {
        return false;
    }
    IntSymbol *left = dynamic_cast<IntSymbol *>(vec[0].get());
    if (left == nullptr) {
        return false;
    }
    AddSymbol *add = dynamic_cast<AddSymbol *>(vec[1].get());
    if (add == nullptr) {
        return false;
    }
    IntSymbol *right = dynamic_cast<IntSymbol *>(vec[2].get());
    if (right == nullptr) {
        return false;
    }
    std::unique_ptr<Symbol> res =
        std::make_unique<AddExpression>(*left, *add, *right);
    vec.clear();
    vec.push_back(std::move(res));
    return true;
}

int main() {
    Parser p{{IntSymbol::shift, AddSymbol::shift}, {createAddExpression}};
    auto symbol = p.parse("1+1");
    if (AddExpression *add = dynamic_cast<AddExpression *>(symbol.get())) {
        std::cout << add->left.val << '+' << add->right.val << '\n';
    }
}