#pragma once

#include <ctre.hpp>
#include <iostream>
#include <variant>

struct token {
    std::string_view str;
    friend std::ostream &operator<<(std::ostream &out, token t) {
        return out << t.str;
    }
};

template <size_t N> struct fixed_string { char str[N]{}; };

template <typename... Tokens> class lexer {
private:
    using Variant = std::variant<Tokens...>;

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

    template <typename CurrToken, typename... RemainingTokens>
    constexpr static Variant get_token(const auto &match) {
        auto m = match.template get<CurrToken::capture_name>();
        if constexpr (sizeof...(RemainingTokens) == 0) {
            return CurrToken{m};
        } else {
            if (m) {
                return CurrToken{m};
            } else {
                return get_token<RemainingTokens...>(match);
            }
        }
    }

    constexpr static auto regex = gen_regex();
    using MatchType = decltype(ctre::tokenize<regex.str>(std::string_view{""}));
    using IterBegin =
        decltype(ctre::tokenize<regex.str>(std::string_view{""}).begin());
    MatchType tokens;

public:
    struct EndIterator {};
    struct BeginIterator {
        IterBegin it;
        constexpr auto operator*() { return get_token<Tokens...>(*it); }
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