#pragma once

#include "control_flow.h"
#include "expression.h"
#include "parser/parser.h"

inline auto parse_expression(std::string_view s) {
    return parse(Terminals<IfToken, Identifier, IntegerToken, OpenParenToken,
                           CloseParenToken, OpenBraceToken, CloseBraceToken,
                           MultToken, AddToken, EqlToken>{},
                 Symbols<IfExpression, IfCondition, Integer, Expression,
                         MultExpression, AddExpression, Assignment>{},
                 s);
}