#pragma once

#include "parser/parser.h"
#include "tokens.h"
#include <charconv>
#include <map>

struct Integer {
    int data;
    Integer(IntegerToken str) {
        std::from_chars(str.str.data(), str.str.data() + str.str.size(), data);
    }
    friend std::ostream &operator<<(std::ostream &out, const Integer &d) {
        return out << "Integer(" << d.data << ")";
    }
};

template <> struct SymbolTraits<Integer> {
    using Constructors = ConstructorTraits<ConstructorParams<IntegerToken>>;
    using ConstructorsNextSymbol =
        ConstructorTraits<MultToken, AddToken, CloseParenToken,
                          CloseBraceToken>;
};

struct AddExpression;
std::ostream &operator<<(std::ostream &out, const AddExpression &a);
int evaluate_add_expression(const AddExpression &a,
                            const std::map<std::string, int> &variables);

struct Expression {
    std::variant<std::unique_ptr<AddExpression>, Identifier, Integer> data;
    explicit Expression(Identifier i) : data(i) {}
    explicit Expression(Integer d) : data(d) {}
    Expression(OpenParenToken, AddExpression &&a, CloseParenToken)
        : data(std::make_unique<AddExpression>(std::move(a))) {}
    friend std::ostream &operator<<(std::ostream &out, const Expression &e) {
        if (std::holds_alternative<Identifier>(e.data)) {
            return out << "Expression(" << std::get<Identifier>(e.data).str
                       << ")";
        } else if (std::holds_alternative<Integer>(e.data)) {
            return out << "Expression(" << std::get<Integer>(e.data).data
                       << ")";
        }
        return out << "Expression("
                   << *std::get<std::unique_ptr<AddExpression>>(e.data) << ")";
    }
    int evaluate(const std::map<std::string, int> &variables) const {
        if (std::holds_alternative<Identifier>(data)) {
            return variables.at(std::string{std::get<Identifier>(data).str});
        } else if (std::holds_alternative<Integer>(data)) {
            return std::get<Integer>(data).data;
        } else {
            return evaluate_add_expression(
                *std::get<std::unique_ptr<AddExpression>>(data), variables);
        }
    }
};

template <> struct SymbolTraits<Expression> {
    using Constructors = ConstructorTraits<
        ConstructorParams<Identifier>, ConstructorParams<Integer>,
        ConstructorParams<OpenParenToken, AddExpression, CloseParenToken>>;
    using ConstructorsNextSymbol =
        ConstructorTraits<MultToken, AddToken, CloseParenToken,
                          CloseBraceToken>;
};

struct MultExpression {
    std::unique_ptr<MultExpression> m;
    Expression e;
    MultExpression(Expression e) : e(std::move(e)) {}
    MultExpression(MultExpression m, MultToken, Expression e)
        : m(std::make_unique<MultExpression>(std::move(m))), e(std::move(e)) {}

    friend std::ostream &operator<<(std::ostream &out,
                                    const MultExpression &other) {
        if (other.m) {
            return out << "MultExpression(" << *other.m << "*" << other.e
                       << ")";
        }
        return out << "MultExpression(" << other.e << ")";
    }

    int evaluate(const std::map<std::string, int> &variables) const {
        if (m) {
            return m->evaluate(variables) * e.evaluate(variables);
        } else {
            return e.evaluate(variables);
        }
    }
};

template <> struct SymbolTraits<MultExpression> {
    using Constructors = ConstructorTraits<
        ConstructorParams<MultExpression, MultToken, Expression>,
        ConstructorParams<Expression>>;
    using ConstructorsNextSymbol =
        ConstructorTraits<MultToken, AddToken, CloseParenToken,
                          CloseBraceToken>;
};

struct AddExpression {
    std::unique_ptr<AddExpression> a;
    MultExpression m;
    AddExpression(MultExpression m) : m(std::move(m)) {}
    AddExpression(AddExpression a, AddToken, MultExpression m)
        : a(std::make_unique<AddExpression>(std::move(a))), m(std::move(m)) {}

    friend std::ostream &operator<<(std::ostream &out,
                                    const AddExpression &other) {
        if (other.a) {
            return out << "AddExpression(" << *other.a << "+" << other.m << ")";
        }
        return out << "AddExpression(" << other.m << ")";
    }
    int evaluate(const std::map<std::string, int> &variables) const {
        if (a) {
            return a->evaluate(variables) + m.evaluate(variables);
        } else {
            return m.evaluate(variables);
        }
    }
};

int evaluate_add_expression(const AddExpression &a,
                            const std::map<std::string, int> &variables) {
    return a.evaluate(variables);
}

template <> struct SymbolTraits<AddExpression> {
    using Constructors = ConstructorTraits<
        ConstructorParams<AddExpression, AddToken, MultExpression>,
        ConstructorParams<MultExpression>>;
    using ConstructorsNextSymbol =
        ConstructorTraits<AddToken, CloseParenToken, CloseBraceToken>;
};

struct Assignment {
    Identifier i;
    AddExpression a;
    Assignment(Identifier i, EqlToken, AddExpression a)
        : i(i), a(std::move(a)) {}

    friend std::ostream &operator<<(std::ostream &out,
                                    const Assignment &other) {
        return out << "Assignment(" << other.i << "=" << other.a << ")";
    }
};

template <> struct SymbolTraits<Assignment> {
    using Constructors = ConstructorTraits<
        ConstructorParams<Identifier, EqlToken, AddExpression>>;
    using ConstructorsNextSymbol = ConstructorTraits<>;
};