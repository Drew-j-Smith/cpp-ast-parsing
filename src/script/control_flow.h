
#include "expression.h"
#include "parser/parser.h"
#include "tokens.h"
#include <optional>

struct IfCondition {
    AddExpression condition;
    IfCondition(IfToken, OpenParenToken, AddExpression condition,
                CloseParenToken)
        : condition(std::move(condition)) {}

    friend std::ostream &operator<<(std::ostream &out,
                                    const IfCondition &other) {
        return out << "if(" << other.condition << ")";
    }
};

template <> struct SymbolTraits<IfCondition> {
    using Constructors =
        ConstructorTraits<ConstructorParams<IfToken, OpenParenToken,
                                            AddExpression, CloseParenToken>>;
    using ConstructorsNextSymbol = ConstructorTraits<OpenBraceToken>;
};

struct IfExpression {
    IfCondition condition;
    AddExpression expression;
    IfExpression(IfCondition condition, OpenBraceToken,
                 AddExpression expression, CloseBraceToken)
        : condition(std::move(condition)), expression(std::move(expression)) {}

    friend std::ostream &operator<<(std::ostream &out,
                                    const IfExpression &other) {
        return out << other.condition << other.expression;
    }

    std::optional<int>
    evaluate(const std::map<std::string, int> &variables) const {
        if (condition.condition.evaluate(variables)) {
            return expression.evaluate(variables);
        }
        return {};
    }
};

template <> struct SymbolTraits<IfExpression> {
    using Constructors =
        ConstructorTraits<ConstructorParams<IfCondition, OpenBraceToken,
                                            AddExpression, CloseBraceToken>>;
    using ConstructorsNextSymbol = ConstructorTraits<>;
};
