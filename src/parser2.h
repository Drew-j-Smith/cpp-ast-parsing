
#include <array>
#include <iostream>
#include <memory>
#include <string_view>
#include <variant>
#include <vector>

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
};

template <typename... Args> struct Terminals {};

template <typename... Args> struct Symbols {};

template <typename Variant, typename... Params>
bool containsPartialMatch_ctor(const std::vector<Variant> &parseStack,
                               const Variant &lookahead,
                               ConstructorParams<Params...>) {
    if constexpr (sizeof...(Params) != 1) {
        std::array<bool, sizeof...(Params) - 1> partialMatches;
        for (auto &match : partialMatches) {
            match = true;
        }
        std::size_t vec_index = 0;
        (
            [&] {
                for (std::size_t arr_index = 0;
                     arr_index < sizeof...(Params) - 1 - vec_index;
                     ++arr_index) {
                    if (parseStack.size() + vec_index + arr_index + 1 <
                        sizeof...(Params)) {
                        // if the index is less than zero
                        partialMatches[arr_index] = false;
                        continue;
                    }
                    std::size_t index = parseStack.size() - sizeof...(Params) +
                                        vec_index + arr_index + 1;
                    if (index < partialMatches.size()) {
                        if (!std::holds_alternative<Params>(
                                parseStack[index])) {
                            partialMatches[arr_index] = false;
                        }
                    } else if (index == partialMatches.size()) {
                        if (!std::holds_alternative<Params>(lookahead)) {
                            partialMatches[arr_index] = false;
                        }
                    }
                }
                vec_index++;
            }(),
            ...);

        for (auto match : partialMatches) {
            if (match) {
                return true;
            }
        }
    }
    return false;
}

template <typename Variant, typename... Ctors>
bool containsPartialMatch_ctors(const std::vector<Variant> &parseStack,
                                const Variant &lookahead,
                                ConstructorTraits<Ctors...>) {
    return (containsPartialMatch_ctor(parseStack, lookahead, Ctors{}) || ...);
}

template <typename Variant, typename Symbol>
bool containsPartialMatch(const std::vector<Variant> &parseStack,
                          const Variant &lookahead) {
    return containsPartialMatch_ctors(
        parseStack, lookahead, typename SymbolTraits<Symbol>::Constructors{});
}

template <typename Variant, typename Symbol, typename... Params>
bool reduce_with_ctor(std::vector<Variant> &parseStack,
                      ConstructorParams<Params...>) {
    if (sizeof...(Params) > parseStack.size()) {
        return false;
    }
    std::size_t index = parseStack.size() - sizeof...(Params);
    if ((std::holds_alternative<Params>(parseStack[index++]) && ...)) {
        std::size_t start = parseStack.size() - sizeof...(Params);
        index = parseStack.size() - sizeof...(Params);
        parseStack[start] =
            Symbol{std::move(std::get<Params>(parseStack[index++]))...};
        parseStack.erase(parseStack.begin() + start + 1,
                         parseStack.begin() + start + sizeof...(Params));
        return true;
    }
    return false;
}

template <typename Variant, typename Symbol, typename... Ctors>
bool reduce_with_ctors(std::vector<Variant> &parseStack,
                       ConstructorTraits<Ctors...>) {
    return (reduce_with_ctor<Variant, Symbol>(parseStack, Ctors{}) || ...);
}

template <typename Variant, typename Symbol>
bool reduce(std::vector<Variant> &parseStack) {
    return reduce_with_ctors<Variant, Symbol>(
        parseStack, typename SymbolTraits<Symbol>::Constructors{});
}

template <typename Variant, typename... SymbolArgs>
void reduce_symbols(std::vector<Variant> &parseStack,
                    const Variant &lookahead) {
    while (!(containsPartialMatch<Variant, SymbolArgs>(parseStack, lookahead) ||
             ...)) {
        if (!(reduce<Variant, SymbolArgs>(parseStack) || ...)) {
            std::cerr << "failed to reduce\n";
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

    while (str.size() > 0) {
        std::cerr << "before\n";
        printStack(parseStack);
        ParseResult<Variant> parseResults{};
        (parseResults + ... +
         TerminalTraits<TerminalArgs, Variant>::shift(str));
        if (std::holds_alternative<std::monostate>(parseResults.variant)) {
            std::cerr << "failed to parse\n";
            throw std::exception{};
        }
        str = parseResults.str;

        std::cerr << "before reduce\n";
        printStack(parseStack);

        reduce_symbols<Variant, SymbolArgs...>(parseStack,
                                               parseResults.variant);
        parseStack.push_back(std::move(parseResults.variant));
    }
    std::cerr << "\nfinal\n";
    printStack(parseStack);
    while (parseStack.size() > 1) {
        if (!(reduce<Variant, SymbolArgs>(parseStack) || ...)) {
            std::cerr << "failed to reduce\n";
            printStack(parseStack);
            throw std::exception{};
        }
    }
    return std::move(parseStack[0]);
}

template <char terminal> struct TermialCharacter {};

template <typename Variant, char terminal>
struct TerminalTraits<TermialCharacter<terminal>, Variant> {
    static ParseResult<Variant> shift(std::string_view str) {
        if (str[0] == terminal) {
            return {TermialCharacter<terminal>{},
                    std::string_view{str.data() + 1, str.size() - 1}};
        }
        return {};
    }
};

template <char c>
std::ostream &operator<<(std::ostream &out, TermialCharacter<c>) {
    return out << c;
}

struct Identifier {
    std::string_view str;
    friend std::ostream &operator<<(std::ostream &out, const Identifier &i) {
        return out << "Identifier(" << i.str << ")";
    }
};

template <typename Variant> struct TerminalTraits<Identifier, Variant> {
    static ParseResult<Variant> shift(std::string_view str) {
        std::size_t index = 0;
        while (index < str.size() && str[index] != '+' && str[index] != '*') {
            ++index;
        }
        if (index == 0) {
            return {};
        }
        return {Identifier{{str.data(), index}},
                std::string_view{str.data() + index, str.size() - index}};
    }
};

struct AddExpression {
    std::unique_ptr<AddExpression> a;
    Identifier i;
    AddExpression(Identifier i) : i(i) {}
    AddExpression(AddExpression a, TermialCharacter<'+'>, Identifier i)
        : a(std::make_unique<AddExpression>(std::move(a))), i(i) {}

    friend std::ostream &operator<<(std::ostream &out, const AddExpression &a) {
        if (a.a) {
            return out << "AddExpression(" << *a.a << "+" << a.i << ")";
        }
        return out << "AddExpression(" << a.i << ")";
    }
};

template <> struct SymbolTraits<AddExpression> {
    using Constructors = ConstructorTraits<
        ConstructorParams<AddExpression, TermialCharacter<'+'>, Identifier>,
        ConstructorParams<Identifier>>;
};

struct MultExpression {
    std::unique_ptr<MultExpression> m;
    AddExpression a;
    MultExpression(AddExpression a) : a(std::move(a)) {}
    MultExpression(MultExpression m, TermialCharacter<'*'>, AddExpression a)
        : m(std::make_unique<MultExpression>(std::move(m))), a(std::move(a)) {}

    friend std::ostream &operator<<(std::ostream &out,
                                    const MultExpression &m) {
        if (m.m) {
            return out << "MultExpression(" << *m.m << "+" << m.a << ")";
        }
        return out << "MultExpression(" << m.a << ")";
    }
};

template <> struct SymbolTraits<MultExpression> {
    using Constructors = ConstructorTraits<
        ConstructorParams<MultExpression, TermialCharacter<'*'>, AddExpression>,
        ConstructorParams<AddExpression>>;
};
