#pragma once

#include <array>
#include <iostream>
#include <memory>
#include <string_view>
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
    // using Constructors = ConstructorTraits<...>;
    // using ConstructorsNextSymbol = ConstructorTraits<...>;
};

template <typename... Args> struct Terminals {};

template <typename... Args> struct Symbols {};

template <typename Variant, typename Symbol, typename... Params,
          typename... NextParams>
bool reduce_with_ctor(std::vector<Variant> &parseStack,
                      const Variant &lookahead, ConstructorParams<Params...>,
                      ConstructorTraits<NextParams...>) {
    if (sizeof...(Params) > parseStack.size()) {
        return false;
    }
    if (!((std::holds_alternative<NextParams>(lookahead) || ...) ||
          std::holds_alternative<std::monostate>(lookahead))) {
        return false;
    }
    std::size_t index = parseStack.size() - sizeof...(Params);
    if ((std::holds_alternative<Params>(parseStack[index++]) && ...)) {
        std::size_t start = parseStack.size() - sizeof...(Params);
        index = parseStack.size() - sizeof...(Params);
        parseStack[start] =
            Symbol{std::move(std::get<Params>(parseStack[index++]))...};
        parseStack.erase(
            parseStack.begin() + static_cast<std::ptrdiff_t>(start + 1),
            parseStack.begin() +
                static_cast<std::ptrdiff_t>(start + sizeof...(Params)));
        return true;
    }
    return false;
}

template <typename Variant, typename Symbol, typename... Ctors,
          typename... NextParams>
bool reduce_with_ctors(std::vector<Variant> &parseStack,
                       const Variant &lookahead, ConstructorTraits<Ctors...>,
                       ConstructorTraits<NextParams...> nextParams) {
    return (reduce_with_ctor<Variant, Symbol>(parseStack, lookahead, Ctors{},
                                              nextParams) ||
            ...);
}

template <typename Variant, typename Symbol>
bool reduce(std::vector<Variant> &parseStack, const Variant &lookahead) {
    return reduce_with_ctors<Variant, Symbol>(
        parseStack, lookahead, typename SymbolTraits<Symbol>::Constructors{},
        typename SymbolTraits<Symbol>::ConstructorsNextSymbol{});
}

template <typename Variant, typename... SymbolArgs>
void reduce_symbols(std::vector<Variant> &parseStack,
                    const Variant &lookahead) {
    while (true) {
        if (!(reduce<Variant, SymbolArgs>(parseStack, lookahead) || ...)) {
            break;
        }
    }
}

template <typename... TerminalArgs, typename... SymbolArgs>
auto parse(Terminals<TerminalArgs...>, Symbols<SymbolArgs...>,
           std::string_view str) {
    using Variant =
        std::variant<std::monostate, TerminalArgs..., SymbolArgs...>;
    std::vector<Variant> parseStack;

    while (!str.empty()) {
        ParseResult<Variant> parseResults{};
        (parseResults + ... +
         TerminalTraits<TerminalArgs, Variant>::shift(str));
        if (std::holds_alternative<std::monostate>(parseResults.variant)) {
            throw std::runtime_error{"failed to parse"};
        }
        str = parseResults.str;

        reduce_symbols<Variant, SymbolArgs...>(parseStack,
                                               parseResults.variant);
        parseStack.push_back(std::move(parseResults.variant));
    }
    while (true) {
        if (!(reduce<Variant, SymbolArgs>(parseStack, std::monostate{}) ||
              ...)) {
            if (parseStack.size() == 1) {
                break;
            }
            printStack(parseStack);
            throw std::runtime_error{"non-empty parse stack"};
        }
    }
    return std::move(parseStack[0]);
}
