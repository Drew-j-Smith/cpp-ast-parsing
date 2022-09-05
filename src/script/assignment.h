#pragma once

#include "expression.h"
#include "parser.h"

struct Assignment {
    Identifier i;
    std::unique_ptr<AddExpression> a;
    Assignment(Identifier i, TermialCharacter<'='>, AddExpression a)
        : i(i), a(std::make_unique<AddExpression>(std::move(a))) {}

    friend std::ostream &operator<<(std::ostream &out,
                                    const Assignment &other) {
        return out << "Assignment(" << other.i << "=" << *other.a << ")";
    }
};

template <> struct SymbolTraits<Assignment> {
    using Constructors = ConstructorTraits<
        ConstructorParams<Identifier, TermialCharacter<'='>, AddExpression>>;
    using ConstructorsNextSymbol = ConstructorTraits<>;
};