#pragma once

#include <iostream>
#include <variant>
#include <vector>

inline std::ostream &operator<<(std::ostream &out, std::monostate) {
    return out << "monostate";
}

template <typename Variant>
void printStack(const std::vector<Variant> &parseStack) {
    std::cerr << "Parse stack:\n";
    for (const auto &el : parseStack) {
        std::visit([](auto &variant) { std::cerr << variant << '\n'; }, el);
    }
    std::cerr << "\n";
}

struct TokenizeError {
    std::string_view msg;
    friend std::ostream &operator<<(std::ostream &out, TokenizeError e) {
        return out << "Incomplete parse, remaining string: \"" << e.msg << "\"";
    }
};
struct ParseError {
    friend std::ostream &operator<<(std::ostream &out, ParseError) {
        return out << "non-empty parse stack";
    }
};

template <typename... Params> struct ConstructorParams {};

template <typename... Params> struct ConstructorTraits {};

template <typename Symbol> struct SymbolTraits {
    using Constructors = ConstructorTraits<>;
    // using ConstructorsNextSymbol = ConstructorTraits<...>;
};

template <typename... Args> struct Terminals {};

template <typename... Args> struct Symbols {};