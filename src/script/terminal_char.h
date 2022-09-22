#pragma once

#include "parser.h"
#include <iostream>

struct OpenParenToken : public token {
    constexpr static ctll::fixed_string capture_name = "OpenParen";
    constexpr static std::string_view regex = "(?<OpenParen>\\()";
};

struct CloseParenToken : public token {
    constexpr static ctll::fixed_string capture_name = "CloseParen";
    constexpr static std::string_view regex = "(?<CloseParen>\\))";
};

struct MultToken : public token {
    constexpr static ctll::fixed_string capture_name = "Mult";
    constexpr static std::string_view regex = "(?<Mult>\\*)";
};

struct AddToken : public token {
    constexpr static ctll::fixed_string capture_name = "Add";
    constexpr static std::string_view regex = "(?<Add>\\+)";
};

struct EqlToken : public token {
    constexpr static ctll::fixed_string capture_name = "Eql";
    constexpr static std::string_view regex = "(?<Eql>=)";
};
