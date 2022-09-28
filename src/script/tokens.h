#pragma once

#include "parser/parser.h"

struct Identifier : public Token {
    constexpr static ctll::fixed_string capture_name = "identifier";
    constexpr static std::string_view regex = "?<identifier>[a-zA-Z]\\w*";
};

struct IfToken : public Token {
    constexpr static ctll::fixed_string capture_name = "if";
    constexpr static std::string_view regex = "?<if>if\\b";
};

struct WhileToken : public Token {
    constexpr static ctll::fixed_string capture_name = "while";
    constexpr static std::string_view regex = "?<while>while\\b";
};

struct IntegerToken : public Token {
    constexpr static ctll::fixed_string capture_name = "number";
    constexpr static std::string_view regex = "?<number>[0-9]+";
};

struct OpenParenToken : public Token {
    constexpr static ctll::fixed_string capture_name = "OpenParen";
    constexpr static std::string_view regex = "?<OpenParen>\\(";
};

struct CloseParenToken : public Token {
    constexpr static ctll::fixed_string capture_name = "CloseParen";
    constexpr static std::string_view regex = "?<CloseParen>\\)";
};

struct OpenBraceToken : public Token {
    constexpr static ctll::fixed_string capture_name = "OpenBrace";
    constexpr static std::string_view regex = "?<OpenBrace>\\{";
};

struct CloseBraceToken : public Token {
    constexpr static ctll::fixed_string capture_name = "CloseBrace";
    constexpr static std::string_view regex = "?<CloseBrace>\\}";
};

struct MultToken : public Token {
    constexpr static ctll::fixed_string capture_name = "Mult";
    constexpr static std::string_view regex = "?<Mult>\\*";
};

struct AddToken : public Token {
    constexpr static ctll::fixed_string capture_name = "Add";
    constexpr static std::string_view regex = "?<Add>\\+";
};

struct EqlToken : public Token {
    constexpr static ctll::fixed_string capture_name = "Eql";
    constexpr static std::string_view regex = "?<Eql>=";
};

struct SemicolonToken : public Token {
    constexpr static ctll::fixed_string capture_name = "Semicolon";
    constexpr static std::string_view regex = "?<Semicolon>;";
};

struct CommaToken : public Token {
    constexpr static ctll::fixed_string capture_name = "Comma";
    constexpr static std::string_view regex = "?<Comma>,";
};

struct OpenSquareBraceToken : public Token {
    constexpr static ctll::fixed_string capture_name = "OpenSquareBrace";
    constexpr static std::string_view regex = "?<OpenSquareBrace>\\[";
};

struct CloseSquareBraceToken : public Token {
    constexpr static ctll::fixed_string capture_name = "CloseSquareBrace";
    constexpr static std::string_view regex = "?<CloseSquareBrace>\\]";
};