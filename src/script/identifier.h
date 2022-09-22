#pragma once

#include "parser.h"
#include <iostream>
#include <string_view>

struct Identifier : public token {
    constexpr static ctll::fixed_string capture_name = "identifier";
    constexpr static std::string_view regex =
        "\\s*(?<identifier>[a-zA-Z][a-zA-Z0-9_]*)\\s*";
};