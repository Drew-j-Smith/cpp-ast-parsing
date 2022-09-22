#pragma once

#include "expression.h"
#include "parser/parser.h"

struct Assignment {
    Identifier i;
    std::unique_ptr<AddExpression> a;
    Assignment(Identifier i, EqlToken, AddExpression a)
        : i(i), a(std::make_unique<AddExpression>(std::move(a))) {}

    friend std::ostream &operator<<(std::ostream &out,
                                    const Assignment &other) {
        return out << "Assignment(" << other.i << "=" << *other.a << ")";
    }
};

template <> struct SymbolTraits<Assignment> {
    using Constructors = ConstructorTraits<
        ConstructorParams<Identifier, EqlToken, AddExpression>>;
    using ConstructorsNextSymbol = ConstructorTraits<>;
};