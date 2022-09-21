
#include <ctre.hpp>
#include <iostream>
#include <variant>

struct token {
    std::string_view str;
};

template <size_t N> struct fixed_string { char str[N]{}; };

template <typename Variant, typename CurrToken, typename... Tokens>
constexpr Variant get_token(const auto &match) {
    auto m = match.template get<CurrToken::capture_name>();
    if constexpr (sizeof...(Tokens) == 0) {
        return CurrToken{m};
    } else {
        if (m) {
            return CurrToken{m};
        } else {
            return get_token<Variant, Tokens...>(match);
        }
    }
}

template <typename... Tokens> class lexer {
private:
    constexpr static auto gen_regex() {
        fixed_string<(Tokens::regex.size() + ...) + sizeof...(Tokens) * 3>
            res{};
        auto str_itr = std::begin(res.str);
        (
            [&]() {
                *(str_itr++) = '(';
                for (auto c : Tokens::regex) {
                    *(str_itr++) = c;
                }
                *(str_itr++) = ')';
                *(str_itr++) = '|';
            }(),
            ...);
        *(--str_itr) = '\0';
        return res;
    }

    constexpr static auto regex = gen_regex();
    using MatchType = decltype(ctre::tokenize<regex.str>(std::string_view{""}));
    using IterBegin =
        decltype(ctre::tokenize<regex.str>(std::string_view{""}).begin());
    using Variant = std::variant<Tokens...>;

    MatchType tokens;

public:
    struct EndIterator {};
    struct BeginIterator {
        IterBegin it;
        constexpr auto operator*() {
            return get_token<Variant, Tokens...>(*it);
        }
        constexpr auto operator++() { return ++it; }
        constexpr auto operator++(int) { return it++; }
        constexpr auto operator==(EndIterator) {
            return it == ctre::regex_end_iterator{};
        }
        constexpr auto operator!=(EndIterator) {
            return it != ctre::regex_end_iterator{};
        }
    };

    constexpr lexer(std::string_view str)
        : tokens(ctre::tokenize<regex.str>(str)) {}

    constexpr auto begin() { return BeginIterator{tokens.begin()}; }

    constexpr auto end() { return EndIterator{}; }
};

struct identifier : public token {
    constexpr static ctll::fixed_string capture_name = "identifier";
    constexpr static std::string_view regex =
        "\\s*(?<identifier>[a-zA-Z][a-zA-Z0-9_]*)\\s*";
};
struct number : public token {
    constexpr static ctll::fixed_string capture_name = "number";
    constexpr static std::string_view regex = "\\s*(?<number>[0-9]+)\\s*";
};

int main() {
    std::string_view str = "test\t\n test 89";
    const char *end = str.data();
    for (const auto token : lexer<identifier, number>{str}) {
        std::visit(
            [&](auto &&arg) {
                std::cout << arg.str << '.';
                end = arg.str.data() + arg.str.size();
            },
            token);
    }
    if (end != str.data() + str.size()) {
        std::cout << "incomplete parse";
    }
}