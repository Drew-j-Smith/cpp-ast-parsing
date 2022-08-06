
#include <charconv>
#include <iostream>
#include <memory>
#include <optional>
#include <string_view>
#include <system_error>
#include <variant>
#include <vector>

template <typename... Symbols> class Parser {
public:
    using Variant = std::variant<Symbols...>;
    using VariantPtr = std::unique_ptr<Variant>;
    using TerminalFp = VariantPtr (*)(std::string_view &);
    using RuleFp = bool (*)(std::vector<VariantPtr> &);

private:
    std::vector<TerminalFp> terminalSymbols;
    std::vector<RuleFp> rules;

    bool reduce(std::vector<VariantPtr> &parseStack) {
        for (auto rule : rules) {
            if (rule(parseStack)) {
                return true;
            }
        }
        return false;
    }

    bool shift(std::vector<VariantPtr> &parseStack, std::string_view &str) {
        for (auto terminal : terminalSymbols) {
            if (auto symbol = terminal(str)) {
                parseStack.push_back(std::move(symbol));
                return true;
            }
        }
        return false;
    }

public:
    Parser(std::vector<TerminalFp> terminalSymbols, std::vector<RuleFp> rules)
        : terminalSymbols{std::move(terminalSymbols)}, rules{std::move(rules)} {
    }

    VariantPtr parse(std::string_view str) {
        std::vector<VariantPtr> parseStack;
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

struct IntSymbol {
    int val;
    IntSymbol(int val) : val{val} {}

    template <typename ParserType>
    static typename ParserType::VariantPtr shift(std::string_view &str) {
        int result{};

        auto [ptr,
              ec]{std::from_chars(str.data(), str.data() + str.size(), result)};

        if (ec == std::errc{}) {
            str = std::string_view(
                ptr, static_cast<std::size_t>(str.data() + str.size() - ptr));
            return std::make_unique<typename ParserType::Variant>(
                IntSymbol{result});
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

class AddSymbol final {
public:
    template <typename ParserType>
    static typename ParserType::VariantPtr shift(std::string_view &str) {
        if (str[0] == '+') {
            str = std::string_view(str.data() + 1, str.size() - 1);
            return std::make_unique<typename ParserType::Variant>(AddSymbol{});
        }
        return {};
    }
};

struct AddExpression {
    IntSymbol left;
    AddSymbol add;
    IntSymbol right;

    AddExpression(IntSymbol left, AddSymbol add, IntSymbol right)
        : left(left), add(add), right(right) {}
};

template <typename ParserType>
bool createAddExpression(std::vector<typename ParserType::VariantPtr> &vec) {
    if (vec.size() != 3) {
        return false;
    }
    auto left = std::get_if<IntSymbol>(vec[0].get());
    auto add = std::get_if<AddSymbol>(vec[1].get());
    auto right = std::get_if<IntSymbol>(vec[2].get());
    if (!left || !add || !right) {
        return false;
    }
    typename ParserType::VariantPtr res =
        std::make_unique<typename ParserType::Variant>(
            AddExpression{*left, *add, *right});
    vec.clear();
    vec.push_back(std::move(res));
    return true;
}

int main() {
    using ParserType = Parser<IntSymbol, AddSymbol, AddExpression>;

    ParserType p{{IntSymbol::shift<ParserType>, AddSymbol::shift<ParserType>},
                 {createAddExpression<ParserType>}};
    auto symbol = p.parse("1+1");
    if (auto expression = std::get_if<AddExpression>(symbol.get())) {
        std::cout << expression->left.val << '+' << expression->right.val
                  << '\n';
    }
}