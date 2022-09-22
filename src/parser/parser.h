#pragma once

#include <array>
#include <iostream>
#include <memory>
#include <string_view>
#include <variant>
#include <vector>

#include "lexer.h"
#include "reduce.h"
#include "util.h"

template <typename... TerminalArgs, typename... SymbolArgs>
auto parse(Terminals<TerminalArgs...>, Symbols<SymbolArgs...>,
           std::string_view str) {
    using Variant =
        std::variant<std::monostate, TerminalArgs..., SymbolArgs...>;
    std::vector<Variant> parseStack;

    const char *end = str.data();
    for (const auto token : lexer<TerminalArgs...>{str}) {
        Variant lookahead;
        std::visit(
            [&](auto &&arg) {
                lookahead = arg;
                end = arg.str.data() + arg.str.size();
            },
            token);
        reduce_symbols<Variant, SymbolArgs...>(parseStack, lookahead);
        parseStack.push_back(std::move(lookahead));
    }
    if (end != str.data() + str.size()) {
        throw std::runtime_error{"incomplete parse"};
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
