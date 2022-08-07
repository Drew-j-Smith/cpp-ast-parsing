#pragma once

#include <array>
#include <charconv>
#include <iostream>
#include <string_view>
#include <system_error>
#include <variant>
#include <vector>

template <typename T> struct ParserTraits {
    template <typename ParserType>
    static typename ParserType::Variant shift(std::string_view &str);

    template <typename ParserType>
    static bool reduce(std::vector<typename ParserType::Variant> &vec);
};

template <typename... Symbols> class Parser {
public:
    using Variant = std::variant<std::monostate, Symbols...>;
    using TerminalFp = Variant (*)(std::string_view &);
    using RuleFp = bool (*)(std::vector<Variant> &);

private:
    std::array<TerminalFp, sizeof...(Symbols)> shifters;
    std::array<RuleFp, sizeof...(Symbols)> reducers;

    constexpr bool reduce(std::vector<Variant> &parseStack) const {
        for (const auto rule : reducers) {
            if (rule(parseStack)) {
                return true;
            }
        }
        return false;
    }

    constexpr bool shift(std::vector<Variant> &parseStack,
                         std::string_view &str) const {
        for (const auto shifter : shifters) {
            auto symbol = shifter(str);
            if (!std::holds_alternative<std::monostate>(symbol)) {
                parseStack.push_back(std::move(symbol));
                return true;
            }
        }
        return false;
    }

    static void printParseStack(const std::vector<Variant> &parseStack) {
        std::cout << "Parse Stack:\n";
        for (const auto &v : parseStack) {
            std::visit([](auto &&arg) { std::cout << arg << '\n'; }, v);
        }
        std::cout << "\n";
    }

public:
    constexpr Parser()
        : shifters{ParserTraits<Symbols>::template shift<
              Parser<Symbols...>>...},
          reducers{
              ParserTraits<Symbols>::template reduce<Parser<Symbols...>>...} {}

    Variant parse(std::string_view str) const {
        std::vector<Variant> parseStack;
        while (str.size() > 0) {
            printParseStack(parseStack);
            if (!shift(parseStack, str)) {
                std::cout << "Error parsing\n";
                return {};
            }
            while (reduce(parseStack))
                ;
        }
        if (parseStack.size() > 1) {
            std::cout << "incomplete parse\n";
        }
        return std::move(parseStack[0]);
    }
};

template <typename T, typename ParserType, typename... Params>
constexpr static bool create(std::vector<typename ParserType::Variant> &vec,
                             int start) {
    if (start < 0) {
        return false;
    }
    int index = start - 1;
    if ((std::holds_alternative<Params>(
             vec[static_cast<std::size_t>(++index)]) &&
         ...)) {
        index = start - 1;
        vec[static_cast<std::size_t>(start)] = T{std::move(
            std::get<Params>(vec[static_cast<std::size_t>(++index)]))...};
        vec.erase(vec.begin() + start + 1,
                  vec.begin() + start + sizeof...(Params));
        return true;
    }
    return false;
}

template <typename T, typename ParserType, typename... Params>
constexpr static bool create(std::vector<typename ParserType::Variant> &vec) {
    int start = static_cast<int>(vec.size() - sizeof...(Params));
    return create<T, ParserType, Params...>(vec, start);
}

template <typename T, typename ParserType, typename Lookahead,
          typename... Params>
constexpr static bool
create_with_lookahead(std::vector<typename ParserType::Variant> &vec) {
    int start = static_cast<int>(vec.size() - sizeof...(Params)) - 1;
    return std::holds_alternative<Lookahead>(vec.back()) &&
           create<T, ParserType, Params...>(vec, start);
}

template <typename T, typename ParserType, typename Lookbehind,
          typename... Params>
constexpr static bool
create_with_lookbehind(std::vector<typename ParserType::Variant> &vec) {
    int start = static_cast<int>(vec.size() - sizeof...(Params));
    if (start - 1 < 0) {
        return false;
    }
    return std::holds_alternative<Lookbehind>(
               vec[static_cast<std::size_t>(start - 1)]) &&
           create<T, ParserType, Params...>(vec, start);
}