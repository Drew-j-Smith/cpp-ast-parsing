#pragma once

#include <array>
#include <string_view>

struct ReservedKeyword {
    static constexpr int MAX_SIZE = 20;
    std::array<char, MAX_SIZE> m_data{};
    std::size_t size;
    consteval ReservedKeyword(const char *data) {
        for (size = 0; data[size] != '\0'; size++) {
            m_data[size] = data[size];
        }
    }
    consteval std::string_view str() const {
        return std::string_view{m_data.data(), size};
    }
};

template <ReservedKeyword keyword> struct Keyword {
    consteval std::string_view str() const { return keyword.str(); }
};
