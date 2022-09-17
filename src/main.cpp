
#include <ctre.hpp>
#include <iostream>
#include <variant>
#include <vector>

struct token {
    std::string_view c;
};

template <size_t N> struct fixed_string { char str[N]{}; };

template <typename... tokens> constexpr auto gen_regex() {
    fixed_string<(tokens::regex.size() + ...) + sizeof...(tokens) * 3> res{};
    auto str_itr = std::begin(res.str);
    (
        [&]() {
            *str_itr = '(';
            ++str_itr;
            for (auto c : tokens::regex) {
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

template <int index, typename Variant, typename currToken, typename... tokens>
Variant get_token(const auto &match) {
    if constexpr (sizeof...(tokens) == 0) {
        return currToken{match};
    } else {
        if (match.template get<index>()) {
            return currToken{match};
        } else {
            return get_token<index + 1, Variant, tokens...>(match);
        }
    }
}

template <typename Variant, typename IterType> struct tokenize_result {
    std::vector<Variant> token_vec;
    IterType end;
};

template <typename... tokens> auto tokenize(std::string_view v) {
    using Variant = std::variant<tokens...>;
    std::vector<Variant> token_vec;
    auto end = v.begin();

    auto token_range = ctre::tokenize<gen_regex<tokens...>().str>(v);
    for (const auto &match : token_range) {
        if (match) {
            token_vec.push_back(get_token<1, Variant, tokens...>(match));
        }
        end = match.end();
    }
    return tokenize_result<Variant, decltype(end)>{token_vec, end};
}

struct identifier : public token {
    constexpr static std::string_view regex = "[a-z]+";
};
struct number : public token {
    constexpr static std::string_view regex = "[0-9]+";
};
struct whitespace : public token {
    constexpr static std::string_view regex = "\\s+";
};

int main() {
    std::string_view str = "test test";
    auto [tokens, end] = tokenize<identifier, number, whitespace>(str);
    if (str.end() - end > 0) {
        std::cout << "Invalid token at: " << end - str.begin() << '\n';
    }
    for (auto item : tokens) {
        std::visit([](auto &&arg) { std::cout << arg.c << '\n'; }, item);
    }
}