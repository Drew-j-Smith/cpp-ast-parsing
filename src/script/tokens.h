#pragma once

#include "parser/parser.h"

struct Identifier : public Token {
    constexpr static ctll::fixed_string capture_name = "identifier";
    constexpr static std::string_view regex =
        "\\s*(?<identifier>[a-zA-Z]\\w*)\\s*";
};

struct IfToken : public Token {
    constexpr static ctll::fixed_string capture_name = "if";
    constexpr static std::string_view regex = "\\s*(?<if>\\s*if\\b)\\s*";
};

struct IntegerToken : public Token {
    constexpr static ctll::fixed_string capture_name = "number";
    constexpr static std::string_view regex = "\\s*(?<number>[0-9]+)\\s*";
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
