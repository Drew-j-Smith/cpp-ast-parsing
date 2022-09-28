#pragma once

#include "parser/parser.h"

struct Identifier : public Token {
    constexpr static ctll::fixed_string capture_name = "identifier";
    constexpr static std::string_view regex = "?<identifier>[a-zA-Z]\\w*";
};

struct IfToken : public Token {
    constexpr static ctll::fixed_string capture_name = "if";
    constexpr static std::string_view regex = "?<if>\\s*if\\b\\s*";
};

struct IntegerToken : public Token {
    constexpr static ctll::fixed_string capture_name = "number";
    constexpr static std::string_view regex = "?<number>[0-9]+";
};

struct OpenParenToken : public Token {
    constexpr static ctll::fixed_string capture_name = "OpenParen";
    constexpr static std::string_view regex = "?<OpenParen>\\s*\\(\\s*";
};

struct CloseParenToken : public Token {
    constexpr static ctll::fixed_string capture_name = "CloseParen";
    constexpr static std::string_view regex = "?<CloseParen>\\s*\\)\\s*";
};

struct OpenBraceToken : public Token {
    constexpr static ctll::fixed_string capture_name = "OpenBrace";
    constexpr static std::string_view regex = "?<OpenBrace>\\s*\\{\\s*";
};

struct CloseBraceToken : public Token {
    constexpr static ctll::fixed_string capture_name = "CloseBrace";
    constexpr static std::string_view regex = "?<CloseBrace>\\s*\\}\\s*";
};

struct MultToken : public Token {
    constexpr static ctll::fixed_string capture_name = "Mult";
    constexpr static std::string_view regex = "?<Mult>\\s*\\*\\s*";
};

struct AddToken : public Token {
    constexpr static ctll::fixed_string capture_name = "Add";
    constexpr static std::string_view regex = "?<Add>\\s*\\+\\s*";
};

struct EqlToken : public Token {
    constexpr static ctll::fixed_string capture_name = "Eql";
    constexpr static std::string_view regex = "?<Eql>\\s*=\\s*";
};

struct SemicolonToken : public Token {
    constexpr static ctll::fixed_string capture_name = "Semicolon";
    constexpr static std::string_view regex = "?<Semicolon>\\s*;\\s*";
};
