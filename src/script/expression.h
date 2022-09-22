#pragma once

#include "parser/parser.h"
#include "tokens.h"
#include <charconv>
#include <map>

struct Double {
    double data;
    Double(DoubleToken str) {
        std::from_chars(str.str.data(), str.str.data() + str.str.size(), data);
    }
    friend std::ostream &operator<<(std::ostream &out, const Double &d) {
        return out << "Double(" << d.data << ")";
    }
};

template <> struct SymbolTraits<Double> {
    using Constructors = ConstructorTraits<ConstructorParams<DoubleToken>>;
    using ConstructorsNextSymbol =
        ConstructorTraits<MultToken, AddToken, CloseParenToken>;
};

struct AddExpression;
std::ostream &operator<<(std::ostream &out, const AddExpression &a);
double evaluate_add_expression(const AddExpression &a,
                               const std::map<std::string, double> &variables);

struct Expression {
    std::variant<std::unique_ptr<AddExpression>, Identifier, Double> data;
    explicit Expression(Identifier i) : data(i) {}
    explicit Expression(Double d) : data(d) {}
    Expression(OpenParenToken, AddExpression &&a, CloseParenToken)
        : data(std::make_unique<AddExpression>(std::move(a))) {}
    friend std::ostream &operator<<(std::ostream &out, const Expression &e) {
        if (std::holds_alternative<Identifier>(e.data)) {
            return out << "Expression(" << std::get<Identifier>(e.data).str
                       << ")";
        } else if (std::holds_alternative<Double>(e.data)) {
            return out << "Expression(" << std::get<Double>(e.data).data << ")";
        }
        return out << "Expression("
                   << *std::get<std::unique_ptr<AddExpression>>(e.data) << ")";
    }
    double evaluate(const std::map<std::string, double> &variables) const {
        if (std::holds_alternative<Identifier>(data)) {
            return variables.at(std::string{std::get<Identifier>(data).str});
        } else if (std::holds_alternative<Double>(data)) {
            return std::get<Double>(data).data;
        } else {
            return evaluate_add_expression(
                *std::get<std::unique_ptr<AddExpression>>(data), variables);
        }
    }
};

template <> struct SymbolTraits<Expression> {
    using Constructors = ConstructorTraits<
        ConstructorParams<Identifier>, ConstructorParams<Double>,
        ConstructorParams<OpenParenToken, AddExpression, CloseParenToken>>;
    using ConstructorsNextSymbol =
        ConstructorTraits<MultToken, AddToken, CloseParenToken>;
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

    double evaluate(const std::map<std::string, double> &variables) const {
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
        ConstructorTraits<MultToken, AddToken, CloseParenToken>;
};

struct AddExpression {
    std::unique_ptr<AddExpression> a;
    std::unique_ptr<MultExpression> m;
    AddExpression(MultExpression m)
        : m(std::make_unique<MultExpression>(std::move(m))) {}
    AddExpression(AddExpression a, AddToken, MultExpression m)
        : a(std::make_unique<AddExpression>(std::move(a))),
          m(std::make_unique<MultExpression>(std::move(m))) {}

    friend std::ostream &operator<<(std::ostream &out,
                                    const AddExpression &other) {
        if (other.a) {
            return out << "AddExpression(" << *other.a << "+" << *other.m
                       << ")";
        }
        return out << "AddExpression(" << *other.m << ")";
    }
    double evaluate(const std::map<std::string, double> &variables) const {
        if (a) {
            return a->evaluate(variables) + m->evaluate(variables);
        } else {
            return m->evaluate(variables);
        }
    }
};

double evaluate_add_expression(const AddExpression &a,
                               const std::map<std::string, double> &variables) {
    return a.evaluate(variables);
}

template <> struct SymbolTraits<AddExpression> {
    using Constructors = ConstructorTraits<
        ConstructorParams<AddExpression, AddToken, MultExpression>,
        ConstructorParams<MultExpression>>;
    using ConstructorsNextSymbol = ConstructorTraits<AddToken, CloseParenToken>;
};