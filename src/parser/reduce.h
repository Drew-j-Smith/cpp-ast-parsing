#pragma once

#include "util.h"

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
    while ((reduce<Variant, SymbolArgs>(parseStack, lookahead) || ...))
        ;
}