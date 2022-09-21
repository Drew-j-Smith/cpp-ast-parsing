
#include <ctre.hpp>
#include <iostream>
#include <variant>
#include <vector>

struct token {
    std::string_view c;
};

template <size_t N> struct fixed_string { char str[N]{}; };

template <typename... Tokens> constexpr auto gen_regex() {
    fixed_string<(Tokens::regex.size() + ...) + sizeof...(Tokens) * 3> res{};
    auto str_itr = std::begin(res.str);
    (
        [&]() {
            *str_itr = '(';
            ++str_itr;
            for (auto c : Tokens::regex) {
                *str_itr = c;
                ++str_itr;
            }
            *str_itr = ')';
            ++str_itr;
            *str_itr = '|';
            ++str_itr;
        }(),
        ...);
    --str_itr;
    *str_itr = '\0';
    return res;
}

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

template <typename Variant, typename IterType> struct tokenize_result {
    std::vector<Variant> token_vec;
    IterType end;
};

template <typename... tokens> constexpr auto tokenize(std::string_view v) {
    using Variant = std::variant<tokens...>;
    std::vector<Variant> token_vec;
    auto end = v.begin();

    auto token_range = ctre::tokenize<gen_regex<tokens...>().str>(v);
    for (const auto &match : token_range) {
        if (match) {
            token_vec.push_back(get_token<Variant, tokens...>(match));
        }
        end = match.end();
    }
    return tokenize_result<Variant, decltype(end)>{token_vec, end};
}

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
    std::string_view str = "test\t\n test 89 ";
    auto [tokens, end] = tokenize<identifier, number>(str);
    if (str.end() != end) {
        std::cout << "Invalid token at: " << end - str.begin() << '\n';
    }
    for (auto item : tokens) {
        std::visit([](auto &&arg) { std::cout << arg.c << '.'; }, item);
    }
}