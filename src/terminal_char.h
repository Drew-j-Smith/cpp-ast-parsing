#pragma once

#include "parser.h"
#include <iostream>

template <char terminal> struct TermialCharacter {};

template <typename Variant, char terminal>
struct TerminalTraits<TermialCharacter<terminal>, Variant> {
    static ParseResult<Variant> shift(std::string_view str) {
        if (str[0] == terminal) {
            return {TermialCharacter<terminal>{},
                    std::string_view{str.data() + 1, str.size() - 1}};
        }
        return {};
    }
};

template <char c>
std::ostream &operator<<(std::ostream &out, TermialCharacter<c>) {
    return out << c;
}