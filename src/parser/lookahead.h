#pragma once

#include "util.h"
#include <array>

template <typename... Args> struct LookaheadTraits {

    template <typename VariantType>
    constexpr void
    updateLookahead(const std::array<std::array<bool, sizeof...(Args)>,
                                     sizeof...(Args)> &table) {}

    constexpr std::array<std::array<bool, sizeof...(Args)>, sizeof...(Args)>
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