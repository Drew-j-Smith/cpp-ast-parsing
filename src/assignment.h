#pragma once

#include "expression.h"
#include "parser.h"

struct Assignment {
    Identifier i;
    std::unique_ptr<AddExpression> a;
    Assignment(Identifier i, TermialCharacter<'='>, AddExpression a)
        : a(std::make_unique<AddExpression>(std::move(a))), i(i) {}

    friend std::ostream &operator<<(std::ostream &out, const Assignment &a) {
        return out << "Assignment(" << a.i << "=" << *a.a << ")";
    }
};

template <> struct SymbolTraits<Assignment> {
    using Constructors = ConstructorTraits<
        ConstructorParams<Identifier, TermialCharacter<'='>, AddExpression>>;
    using ConstructorsNextSymbol = ConstructorTraits<ConstructorParams<>>;
};

inline auto parse_expression(std::string_view s) {
    return parse(
        Terminals<Identifier, TermialCharacter<'+'>, TermialCharacter<'*'>,
                  TermialCharacter<'('>, TermialCharacter<')'>,
                  TermialCharacter<'='>>{},
        Symbols<Expression, MultExpression, AddExpression, Assignment>{}, s);
}