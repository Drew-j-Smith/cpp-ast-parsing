#pragma once

#include "parser.h"
#include <memory>

struct WhiteSpace {
    friend std::ostream &operator<<(std::ostream &out, const WhiteSpace &) {
        return out << ' ';
    }
};

template <> struct ParserTraits<WhiteSpace> {
    template <typename ParserType>
    static typename ParserType::Variant shift(std::string_view &str) {
        std::size_t index = 0;
        while (str[index] == ' ') {
            ++index;
        }
        str = std::string_view{str.data() + index, str.size() - index};
        return {};
    }

    template <typename ParserType>
    static bool
    reduce([[maybe_unused]] std::vector<typename ParserType::Variant> &vec) {
        return false;
    }
};

struct Identifier {
    std::string_view str;
    constexpr Identifier(std::string_view str) : str(str) {}

    friend std::ostream &operator<<(std::ostream &out, const Identifier &i) {
        return out << "Identifier(" << i.str << ")";
    }
};

template <> struct ParserTraits<Identifier> {
    template <typename ParserType>
    static typename ParserType::Variant shift(std::string_view &str) {
        std::size_t index = 0;
        while (index < str.size() && str[index] != ' ' && str[index] != '&' &&
               str[index] != '|' && str[index] != '!') {
            ++index;
        }
        if (index == 0) {
            return {};
        }
        Identifier res{{str.data(), index}};
        str = std::string_view{str.data() + index, str.size() - index};
        return res;
    }

    template <typename ParserType>
    static bool
    reduce([[maybe_unused]] std::vector<typename ParserType::Variant> &vec) {
        return false;
    }
};

struct NotSymbol {
    friend std::ostream &operator<<(std::ostream &out, const NotSymbol &) {
        return out << '!';
    }
};

template <> struct ParserTraits<NotSymbol> {
    template <typename ParserType>
    static typename ParserType::Variant shift(std::string_view &str) {
        if (str[0] == '!') {
            str = std::string_view(str.data() + 1, str.size() - 1);
            return NotSymbol{};
        }
        return {};
    }

    template <typename ParserType>
    static bool
    reduce([[maybe_unused]] std::vector<typename ParserType::Variant> &) {
        return false;
    }
};

struct InvertableIdentifier {
    bool inverted;
    Identifier identifier;
    InvertableIdentifier([[maybe_unused]] NotSymbol notSymbol,
                         Identifier identifier)
        : inverted(true), identifier(identifier) {}
    InvertableIdentifier(Identifier identifier)
        : inverted(false), identifier(identifier) {}

    friend std::ostream &operator<<(std::ostream &out,
                                    const InvertableIdentifier &i) {
        if (i.inverted) {
            return out << '!' << i.identifier;
        } else {
            return out << i.identifier;
        }
    }
};

template <> struct ParserTraits<InvertableIdentifier> {
    template <typename ParserType>
    static typename ParserType::Variant
    shift([[maybe_unused]] std::string_view &str) {
        return {};
    }

    template <typename ParserType>
    static bool reduce(std::vector<typename ParserType::Variant> &vec) {
        return create<InvertableIdentifier, ParserType, NotSymbol, Identifier>(
                   vec) ||
               create<InvertableIdentifier, ParserType, Identifier>(vec);
    }
};

struct AndSymbol {
    friend std::ostream &operator<<(std::ostream &out, const AndSymbol &) {
        return out << '&';
    }
};

template <> struct ParserTraits<AndSymbol> {
    template <typename ParserType>
    static typename ParserType::Variant shift(std::string_view &str) {
        if (str[0] == '&') {
            str = std::string_view(str.data() + 1, str.size() - 1);
            return AndSymbol{};
        }
        return {};
    }

    template <typename ParserType>
    static bool
    reduce([[maybe_unused]] std::vector<typename ParserType::Variant> &) {
        return false;
    }
};

struct AndExpression {
    std::unique_ptr<AndExpression> left;
    InvertableIdentifier right;
    AndExpression(InvertableIdentifier identifier) : right(identifier) {}
    AndExpression(AndExpression left, [[maybe_unused]] AndSymbol op,
                  InvertableIdentifier right)
        : left(std::make_unique<AndExpression>(std::move(left))), right(right) {
    }

    friend std::ostream &operator<<(std::ostream &out,
                                    const AndExpression &expression) {
        if (expression.left) {
            return out << "AndExpression(" << *expression.left << ','
                       << expression.right << ")";
        } else {
            return out << expression.right;
        }
    }
};

template <> struct ParserTraits<AndExpression> {
    template <typename ParserType>
    static typename ParserType::Variant
    shift([[maybe_unused]] std::string_view &str) {
        return {};
    }

    template <typename ParserType>
    static bool reduce(std::vector<typename ParserType::Variant> &vec) {
        return create<AndExpression, ParserType, AndExpression, AndSymbol,
                      InvertableIdentifier>(vec) ||
               create_with_lookahead<AndExpression, ParserType, AndSymbol,
                                     InvertableIdentifier>(vec) ||
               create_with_lookbehind<AndExpression, ParserType, OrSymbol,
                                      InvertableIdentifier>(vec);
    }
};

struct OrSymbol {
    friend std::ostream &operator<<(std::ostream &out, const OrSymbol &) {
        return out << '|';
    }
};

template <> struct ParserTraits<OrSymbol> {
    template <typename ParserType>
    static typename ParserType::Variant shift(std::string_view &str) {
        if (str[0] == '|') {
            str = std::string_view(str.data() + 1, str.size() - 1);
            return OrSymbol{};
        }
        return {};
    }

    template <typename ParserType>
    static bool
    reduce([[maybe_unused]] std::vector<typename ParserType::Variant> &) {
        return false;
    }
};

struct OrExpression {
    std::unique_ptr<OrExpression> left;
    AndExpression right;
    OrExpression(AndExpression right) : right(std::move(right)) {}
    OrExpression(OrExpression left, [[maybe_unused]] OrSymbol op,
                 AndExpression right)
        : left(std::make_unique<OrExpression>(std::move(left))),
          right(std::move(right)) {}
    friend std::ostream &operator<<(std::ostream &out,
                                    const OrExpression &expression) {
        if (expression.left) {
            return out << "OrExpression(" << *expression.left << ','
                       << expression.right << ")";
        } else {
            return out << expression.right;
        }
    }
};

template <> struct ParserTraits<OrExpression> {
    template <typename ParserType>
    static typename ParserType::Variant
    shift([[maybe_unused]] std::string_view &str) {
        return {};
    }

    template <typename ParserType>
    static bool reduce(std::vector<typename ParserType::Variant> &vec) {
        return create<OrExpression, ParserType, OrExpression, OrSymbol,
                      AndExpression>(vec) ||
               create_with_lookahead<OrExpression, ParserType, OrSymbol,
                                     AndExpression>(vec);
    }
};