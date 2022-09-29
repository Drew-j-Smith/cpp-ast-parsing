#pragma once

#include <ctre.hpp>
#include <iostream>
#include <variant>

struct Token {
    std::string_view str;
    friend std::ostream &operator<<(std::ostream &out, Token t) {
        return out << t.str;
    }
};

template <typename... Tokens> class Lexer {
private:
    constexpr static auto gen_regex() {
        char res[(Tokens::regex.size() + ...) + sizeof...(Tokens) * 3]{};
        char *str_itr = res;
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
        return ctll::fixed_string{res};
    }

    template <int index, typename CurrToken, typename... RemainingTokens>
    constexpr static std::variant<Tokens...> get_token(const auto &match) {
        auto m = match.template get<index>();
        if constexpr (sizeof...(RemainingTokens) == 0) {
            return CurrToken{m};
        } else {
            if (m) {
                return CurrToken{m};
            } else {
                return get_token<index + 1, RemainingTokens...>(match);
            }
        }
    }

    constexpr static auto regex = gen_regex();
    using MatchType = decltype(ctre::tokenize<regex>(std::string_view{""}));
    using IterBegin =
        decltype(ctre::tokenize<regex>(std::string_view{""}).begin());
    MatchType tokens;

public:
    struct EndIterator {};
    struct BeginIterator {
        IterBegin it;
        constexpr auto operator*() { return get_token<1, Tokens...>(*it); }
        constexpr auto &operator++() {
            ++it;
            return *this;
        }
        constexpr auto operator++(int) { return BeginIterator{it++}; }
        constexpr auto operator==(EndIterator) {
            return it == ctre::regex_end_iterator{};
        }
        constexpr auto operator!=(EndIterator) {
            return it != ctre::regex_end_iterator{};
        }
    };

    constexpr Lexer(std::string_view str)
        : tokens(ctre::tokenize<regex>(str)) {}

    constexpr auto begin() { return BeginIterator{tokens.begin()}; }

    constexpr auto end() { return EndIterator{}; }
};