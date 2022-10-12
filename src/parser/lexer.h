#pragma once

#include <algorithm>
#include <array>
#include <cstring>
#include <iostream>
#include <string_view>
#include <variant>
using namespace std::literals;

enum class TokenType : int {
    OpenParen,
    CloseParen,
    OpenBrace,
    CloseBrace,
    OpenSquareBrace,
    CloseSquareBrace,
    Plus,
    Minus,
    Mult,
    Equal,
    Comma,
    SemiColon,
    EndOfStream,
    If,
    While,
    Identifier,
    Number,
};

struct Token {
    TokenType type;
    std::string_view str;
};

constexpr static char chars[] = "(){}[]+-*=,;";
constexpr static auto keywords = std::array{"if"sv, "while"sv};
constexpr Token parse(std::string_view str) {
    constexpr auto is_word = [](char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
               (c >= '0' && c <= '9');
    };
    constexpr auto is_number = [](char c) { return c >= '0' && c <= '9'; };

    if (auto whitespace = str.find_first_not_of(" \t\n\r");
        whitespace != std::string_view::npos) {
        str.remove_prefix(whitespace);
    }

    if (str.size() == 0) {
        return {TokenType::EndOfStream, str};
    }

    for (auto i = 0ULL; i < sizeof(chars); i++) {
        if (str.front() == chars[i]) {
            return {static_cast<TokenType>(i), {str.data(), 1}};
        }
    }

    for (auto i = 0ULL; i < keywords.size(); i++) {
        if (str.size() > keywords[i].size() && str.starts_with(keywords[i]) &&
            !is_word(str[keywords[i].size()])) {
            return {static_cast<TokenType>(i + sizeof(chars)),
                    {str.data(), keywords[i].size()}};
        }
    }

    if (auto number_end = std::ranges::find_if_not(str, is_number);
        number_end != str.begin()) {
        return {TokenType::Number, {str.begin(), number_end}};
    }

    if (auto identifier_end = std::ranges::find_if_not(str, is_word);
        identifier_end != str.begin()) {
        return {TokenType::Identifier, {str.begin(), identifier_end}};
    }

    return {TokenType::EndOfStream, {}};
}

struct Lexer {
    std::string_view str;
    constexpr Lexer(std::string_view str) : str(str) {}

    struct IteratorSentinel {};
    struct Iterator {
        using value_type = Token;
        using reference = Token &;
        using pointer = Token *;
        using iterator_category = std::input_iterator_tag;
        using difference_type = std::ptrdiff_t;

        std::string_view str;
        Token token;
        constexpr auto &operator*() const { return token; }
        constexpr auto operator->() const { return &token; }
        auto &operator++() {
            token = parse(str);
            str = {token.str.data() + token.str.size(),
                   static_cast<std::size_t>(
                       (str.data() + str.size()) -
                       (token.str.data() + token.str.size()))};
            return *this;
        }
        auto operator++(int) {
            Iterator copy{*this};
            operator++();
            return copy;
        }
        constexpr auto operator==(IteratorSentinel) const {
            return token.type == TokenType::EndOfStream;
        }
    };

    auto begin() { return ++Iterator{str, {}}; }
    constexpr auto end() { return IteratorSentinel{}; }
};

struct Identifier {
    std::string_view str;
};
struct IntegerToken {
    std::string_view str;
};
struct StringLiteralToken {
    std::string_view str;
};
struct IfToken {};
struct WhileToken {};
struct OpenParenToken {};
struct CloseParenToken {};
struct OpenBraceToken {};
struct CloseBraceToken {};
struct MultToken {};
struct AddToken {};
struct SubToken {};
struct EqlToken {};
struct SemicolonToken {};
struct CommaToken {};
struct OpenSquareBraceToken {};
struct CloseSquareBraceToken {};

std::variant<std::monostate, IfToken, WhileToken, Identifier, IntegerToken,
             OpenParenToken, CloseParenToken, OpenBraceToken, CloseBraceToken,
             MultToken, AddToken, SubToken, EqlToken, SemicolonToken,
             CommaToken, OpenSquareBraceToken, CloseSquareBraceToken,
             StringLiteralToken>
tokenToVariant(Token t) {

    switch (t.type) {
    case TokenType::OpenParen:
        return OpenParenToken{};
    case TokenType::CloseParen:
        return CloseParenToken{};
    case TokenType::OpenBrace:
        return OpenBraceToken{};
    case TokenType::CloseBrace:
        return CloseBraceToken{};
    case TokenType::OpenSquareBrace:
        return OpenSquareBraceToken{};
    case TokenType::CloseSquareBrace:
        return CloseSquareBraceToken{};
    case TokenType::Plus:
        return AddToken{};
    case TokenType::Minus:
        return SubToken{};
    case TokenType::Mult:
        return MultToken{};
    case TokenType::Equal:
        return EqlToken{};
    case TokenType::Comma:
        return CommaToken{};
    case TokenType::SemiColon:
        return SemicolonToken{};
    case TokenType::EndOfStream:
        return std::monostate{};
    case TokenType::If:
        return IfToken{};
    case TokenType::While:
        return WhileToken{};
    case TokenType::Identifier:
        return Identifier{t.str};
    case TokenType::Number:
        return IntegerToken{t.str};
    }
    return {};
}
