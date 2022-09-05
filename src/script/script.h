#pragma once

#include "assignment.h"
#include "expression.h"
#include "parser.h"

inline auto parse_expression(std::string_view s) {
    return parse(
        Terminals<Identifier, Double, TermialCharacter<'+'>,
                  TermialCharacter<'*'>, TermialCharacter<'('>,
                  TermialCharacter<')'>, TermialCharacter<'='>>{},
        Symbols<Expression, MultExpression, AddExpression, Assignment>{}, s);
}