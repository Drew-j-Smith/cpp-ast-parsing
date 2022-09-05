#pragma once

#include <array>
#include <iostream>
#include <memory>
#include <string_view>
#include <variant>
#include <vector>

#include "reduce.h"
#include "util.h"

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
