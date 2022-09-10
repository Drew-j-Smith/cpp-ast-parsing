#pragma once

#include "util.h"
#include <array>

template <typename... Args> struct LookaheadTraits {

    template <typename TypeToIndex> constexpr static int getIndex() {
        return 0;
    }

    template <typename First, typename Second>
    constexpr static void updateLookaheadType(
        std::array<std::array<bool, sizeof...(Args)>, sizeof...(Args)> &table) {
        table[getIndex<First>()][getIndex<Second>()] = true;
    }

    template <typename... CtorArgs1, typename... CtorArgs2>
    constexpr static void updateLookaheadCtor_impl(
        std::array<std::array<bool, sizeof...(Args)>, sizeof...(Args)> &table,
        ConstructorParams<CtorArgs1...>, ConstructorParams<CtorArgs2...>) {
        (updateLookaheadType<CtorArgs1, CtorArgs2>(table), ...);
    }

    template <typename... CtorArgs>
    constexpr static void updateLookaheadCtor(
        std::array<std::array<bool, sizeof...(Args)>, sizeof...(Args)> &table,
        ConstructorParams<CtorArgs...>) {
        updateLookaheadCtor_impl(
            table, ConstructorParams<CtorArgs..., std::monostate>{},
            ConstructorParams<std::monostate, CtorArgs...>{});
    }

    template <typename... Constructors>
    constexpr static void updateLookaheadCtors(
        std::array<std::array<bool, sizeof...(Args)>, sizeof...(Args)> &table,
        ConstructorTraits<Constructors...>) {
        (updateLookaheadCtor(table, Constructors{}), ...);
    }

    template <typename VariantType>
    constexpr static void updateLookahead(
        std::array<std::array<bool, sizeof...(Args)>, sizeof...(Args)> &table) {
        updateLookaheadCtors(
            table, typename SymbolTraits<VariantType>::Constructors{});
    }

    constexpr static std::array<std::array<bool, sizeof...(Args)>,
                                sizeof...(Args)>
    genLookaheadTable() {
        std::array<std::array<bool, sizeof...(Args)>, sizeof...(Args)> res;
        (updateLookahead<Args>(res), ...);
        return res;
    }

    constexpr static auto lookaheadTable = genLookaheadTable();

    template <typename VariantType>
    constexpr static bool
    validLookahead(const std::variant<Args...> &variant,
                   const std::variant<Args...> &lookahead) {
        return lookaheadTable[variant.index()][lookahead.index()];
    }
};