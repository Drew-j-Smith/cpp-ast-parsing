#pragma once

#include "parser/parser.h"

struct Identifier : public Token {
    constexpr static std::string_view regex = "[a-zA-Z]\\w*";
};

struct IfToken : public Token {
    constexpr static std::string_view regex = "if\\b";
};

struct WhileToken : public Token {
    constexpr static std::string_view regex = "while\\b";
};

struct IntegerToken : public Token {
    constexpr static std::string_view regex = "[0-9]+";
};

struct OpenParenToken : public Token {
    constexpr static std::string_view regex = "\\(";
};

struct CloseParenToken : public Token {
    constexpr static std::string_view regex = "\\)";
};

struct OpenBraceToken : public Token {
    constexpr static std::string_view regex = "\\{";
};

struct CloseBraceToken : public Token {
    constexpr static std::string_view regex = "\\}";
};

struct MultToken : public Token {
    constexpr static std::string_view regex = "\\*";
};

struct AddToken : public Token {
    constexpr static std::string_view regex = "\\+";
};

struct SubToken : public Token {
    constexpr static std::string_view regex = "\\-";
};

struct EqlToken : public Token {
    constexpr static std::string_view regex = "=";
};

struct SemicolonToken : public Token {
    constexpr static std::string_view regex = ";";
};

struct CommaToken : public Token {
    constexpr static std::string_view regex = ",";
};

struct OpenSquareBraceToken : public Token {
    constexpr static std::string_view regex = "\\[";
};

struct CloseSquareBraceToken : public Token {
    constexpr static std::string_view regex = "\\]";
};