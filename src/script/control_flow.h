
#include "expression.h"
#include "parser/parser.h"

struct IfExpression;
std::ostream &operator<<(std::ostream &out, const IfExpression &other);
void evaluate_if_expression(const IfExpression &i,
                            std::map<std::string, Variable> &variables);
struct WhileExpression;
std::ostream &operator<<(std::ostream &out, const WhileExpression &other);
void evaluate_while_expression(const WhileExpression &w,
                               std::map<std::string, Variable> &variables);

struct Statement {
    std::variant<AddExpression, Assignment, std::unique_ptr<IfExpression>,
                 std::unique_ptr<WhileExpression>>
        data;
    Statement(AddExpression a, SemicolonToken) : data(std::move(a)) {}
    Statement(Assignment a, SemicolonToken) : data(std::move(a)) {}
    explicit Statement(IfExpression &&i)
        : data(std::make_unique<IfExpression>(std::move(i))) {}
    explicit Statement(WhileExpression &&w)
        : data(std::make_unique<WhileExpression>(std::move(w))) {}
    friend std::ostream &operator<<(std::ostream &out, const Statement &other) {
        if (std::holds_alternative<AddExpression>(other.data)) {
            return out << std::get<AddExpression>(other.data) << ';';
        } else if (std::holds_alternative<Assignment>(other.data)) {
            return out << std::get<Assignment>(other.data) << ';';
        } else if (std::holds_alternative<std::unique_ptr<IfExpression>>(
                       other.data)) {
            return out << *std::get<std::unique_ptr<IfExpression>>(other.data)
                       << ';';
        } else {
            return out << *std::get<std::unique_ptr<WhileExpression>>(
                              other.data)
                       << ';';
        }
    }
    void evaluate(std::map<std::string, Variable> &variables) const {
        if (std::holds_alternative<AddExpression>(data)) {
            std::get<AddExpression>(data).evaluate(variables);
        } else if (std::holds_alternative<Assignment>(data)) {
            std::get<Assignment>(data).evaluate(variables);
        } else if (std::holds_alternative<std::unique_ptr<IfExpression>>(
                       data)) {
            evaluate_if_expression(
                *std::get<std::unique_ptr<IfExpression>>(data), variables);
        } else {
            evaluate_while_expression(
                *std::get<std::unique_ptr<WhileExpression>>(data), variables);
        }
    }
};

template <> struct SymbolTraits<Statement> {
    using Constructors =
        ConstructorTraits<ConstructorParams<AddExpression, SemicolonToken>,
                          ConstructorParams<Assignment, SemicolonToken>,
                          ConstructorParams<IfExpression>,
                          ConstructorParams<WhileExpression>>;
    using ConstructorsNextSymbol =
        ConstructorTraits<OpenBraceToken, Identifier, IfToken, WhileToken,
                          IntegerToken, OpenParenToken, CloseBraceToken>;
};

struct Block {
    std::vector<Statement> statements;
    explicit Block(Statement s) { statements.push_back(std::move(s)); }
    Block(Block other, Statement s) : statements(std::move(other.statements)) {
        statements.push_back(std::move(s));
    }
    friend std::ostream &operator<<(std::ostream &out, const Block &other) {
        for (const auto &statement : other.statements) {
            out << statement;
        }
        return out;
    }
    void evaluate(std::map<std::string, Variable> &variables) const {
        for (const auto &statement : statements) {
            statement.evaluate(variables);
        }
    }
};

template <> struct SymbolTraits<Block> {
    using Constructors = ConstructorTraits<ConstructorParams<Block, Statement>,
                                           ConstructorParams<Statement>>;
    using ConstructorsNextSymbol =
        ConstructorTraits<OpenBraceToken, Identifier, IfToken, WhileToken,
                          IntegerToken, OpenParenToken, CloseBraceToken>;
};

// here to prevent the condition from consuming the parens
// i.e parsing to "if(AddExpression)" instead of "if AddExpression"
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
    Block block;
    IfExpression(IfCondition condition, OpenBraceToken, Block block,
                 CloseBraceToken)
        : condition(std::move(condition)), block(std::move(block)) {}

    friend std::ostream &operator<<(std::ostream &out,
                                    const IfExpression &other) {
        return out << other.condition << other.block;
    }

    void evaluate(std::map<std::string, Variable> &variables) const {
        if (std::get<int>(condition.condition.evaluate(variables).data)) {
            block.evaluate(variables);
        }
    }
};

void evaluate_if_expression(const IfExpression &i,
                            std::map<std::string, Variable> &variables) {
    i.evaluate(variables);
}

template <> struct SymbolTraits<IfExpression> {
    using Constructors = ConstructorTraits<
        ConstructorParams<IfCondition, OpenBraceToken, Block, CloseBraceToken>>;
    using ConstructorsNextSymbol =
        ConstructorTraits<OpenBraceToken, Identifier, IfToken, WhileToken,
                          IntegerToken, OpenParenToken>;
};

struct WhileCondition {
    AddExpression condition;
    WhileCondition(WhileToken, OpenParenToken, AddExpression condition,
                   CloseParenToken)
        : condition(std::move(condition)) {}

    friend std::ostream &operator<<(std::ostream &out,
                                    const WhileCondition &other) {
        return out << "while(" << other.condition << ")";
    }
};

template <> struct SymbolTraits<WhileCondition> {
    using Constructors =
        ConstructorTraits<ConstructorParams<WhileToken, OpenParenToken,
                                            AddExpression, CloseParenToken>>;
    using ConstructorsNextSymbol = ConstructorTraits<OpenBraceToken>;
};

struct WhileExpression {
    WhileCondition condition;
    Block block;
    WhileExpression(WhileCondition condition, OpenBraceToken, Block block,
                    CloseBraceToken)
        : condition(std::move(condition)), block(std::move(block)) {}

    friend std::ostream &operator<<(std::ostream &out,
                                    const WhileExpression &other) {
        return out << other.condition << other.block;
    }

    void evaluate(std::map<std::string, Variable> &variables) const {
        while (std::get<int>(condition.condition.evaluate(variables).data)) {
            block.evaluate(variables);
        }
    }
};

void evaluate_while_expression(const WhileExpression &w,
                               std::map<std::string, Variable> &variables) {
    w.evaluate(variables);
}

template <> struct SymbolTraits<WhileExpression> {
    using Constructors =
        ConstructorTraits<ConstructorParams<WhileCondition, OpenBraceToken,
                                            Block, CloseBraceToken>>;
    using ConstructorsNextSymbol =
        ConstructorTraits<OpenBraceToken, Identifier, IfToken, WhileToken,
                          IntegerToken, OpenParenToken>;
};
