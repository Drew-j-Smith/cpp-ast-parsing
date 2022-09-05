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

template <typename Variant> struct ParseResult {
    Variant variant;
    std::string_view str;

    ParseResult &operator+(ParseResult &&other) {
        if (!std::holds_alternative<std::monostate>(other.variant)) {
            str = other.str;
            variant = std::move(other.variant);
        }
        return *this;
    }
};

template <typename Terminal, typename Variant> struct TerminalTraits {
    static ParseResult<Variant> shift(std::string_view);
};

template <typename... Params> struct ConstructorParams {};

template <typename... Params> struct ConstructorTraits {};

template <typename Symbol> struct SymbolTraits {
    using Constructors = ConstructorTraits<>;
    // using ConstructorsNextSymbol = ConstructorTraits<...>;
};

template <typename... Args> struct Terminals {};

template <typename... Args> struct Symbols {};