#pragma once

#include "control_flow.h"
#include "expression.h"
#include "parser/parser.h"

inline auto parse_expression(std::string_view s) {
    return parse(
        Terminals<IfToken, WhileToken, Identifier, IntegerToken, OpenParenToken,
                  CloseParenToken, OpenBraceToken, CloseBraceToken, MultToken,
                  AddToken, SubToken, EqlToken, SemicolonToken, CommaToken,
                  OpenSquareBraceToken, CloseSquareBraceToken,
                  StringLiteralToken>{},
        Symbols<FunctionCall, IfExpression, IfCondition, WhileExpression,
                WhileCondition, Integer, Expression, MultExpression,
                AddExpression, Assignment, Statement, Block,
                FunctionParameters>{},
        s);
}