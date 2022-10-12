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
    using Variant = std::variant<std::monostate, TokenizeError, ParseError,
                                 TerminalArgs..., SymbolArgs...>;
    std::vector<Variant> parseStack;

    const char *end = str.data();
    for (const auto token : Lexer{str}) {
        Variant lookahead = token.toVariant<Variant>();
        if (std::holds_alternative<std::monostate>(lookahead)) {
            break;
        }
        end = token.str.data() + token.str.size();
        reduce_symbols<Variant, SymbolArgs...>(parseStack, lookahead);
        parseStack.push_back(std::move(lookahead));
    }
    if (end != str.data() + str.size()) {
        return Variant{TokenizeError{
            std::string_view{end, static_cast<std::string_view::size_type>(
                                      str.data() + str.size() - end)}}};
    }
    while (true) {
        if (!(reduce<Variant, SymbolArgs>(parseStack, std::monostate{}) ||
              ...)) {
            if (parseStack.size() == 1) {
                break;
            }
            printStack(parseStack);
            return Variant{ParseError{}};
        }
    }
    return std::move(parseStack[0]);
}
