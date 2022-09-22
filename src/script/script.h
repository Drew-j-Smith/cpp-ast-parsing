#pragma once

#include "assignment.h"
#include "expression.h"
#include "parser.h"

inline auto parse_expression(std::string_view s) {
    return parse(Terminals<Identifier, DoubleToken, OpenParenToken,
                           CloseParenToken, MultToken, AddToken, EqlToken>{},
                 Symbols<Double, Expression, MultExpression, AddExpression,
                         Assignment>{},
                 s);
}