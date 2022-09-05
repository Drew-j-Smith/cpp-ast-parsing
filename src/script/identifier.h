#pragma once

#include "parser.h"
#include <iostream>
#include <string_view>

struct Identifier {
    std::string_view str;
    friend std::ostream &operator<<(std::ostream &out, const Identifier &i) {
        return out << "Identifier(" << i.str << ")";
    }
};

template <typename Variant> struct TerminalTraits<Identifier, Variant> {
    static ParseResult<Variant> shift(std::string_view str) {
        std::size_t index = 0;
        if (std::isdigit(str[0])) {
            return {};
        }
        while (index < str.size() && str[index] != '+' && str[index] != '*' &&
               str[index] != '(' && str[index] != ')' && str[index] != '=' &&
               !std::isspace(str[index])) {
            ++index;
        }
        if (index == 0) {
            return {};
        }
        return {Identifier{{str.data(), index}},
                std::string_view{str.data() + index, str.size() - index}};
    }
};