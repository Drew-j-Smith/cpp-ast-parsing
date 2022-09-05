#pragma once

#include "identifier.h"
#include "parser.h"
#include "terminal_char.h"
#include <charconv>
#include <map>

struct Double {
    double data;
    friend std::ostream &operator<<(std::ostream &out, const Double &d) {
        return out << "Double(" << d.data << ")";
    }
};

template <typename Variant> struct TerminalTraits<Double, Variant> {
    static ParseResult<Variant> shift(std::string_view str) {
        double result;
        auto [ptr,
              ec]{std::from_chars(str.data(), str.data() + str.size(), result)};
        if (ec == std::errc{}) {
            return {Double{result},
                    std::string_view{ptr, static_cast<std::size_t>(
                                              str.data() + str.size() - ptr)}};
        } else {
            return {};
        }
    }
};

struct AddExpression;
std::ostream &operator<<(std::ostream &out, const AddExpression &a);
double evaluate_add_expression(const AddExpression &a,
                               const std::map<std::string, double> &variables);

struct Expression {
    std::variant<std::unique_ptr<AddExpression>, Identifier, Double> data;
    explicit Expression(Identifier i) : data(i) {}
    explicit Expression(Double d) : data(d) {}
    Expression(TermialCharacter<'('>, AddExpression &&a, TermialCharacter<')'>)
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
        ConstructorParams<TermialCharacter<'('>, AddExpression,
                          TermialCharacter<')'>>>;
    using ConstructorsNextSymbol =
        ConstructorTraits<TermialCharacter<'*'>, TermialCharacter<'+'>,
                          TermialCharacter<')'>>;
};

struct MultExpression {
    std::unique_ptr<MultExpression> m;
    Expression e;
    MultExpression(Expression e) : e(std::move(e)) {}
    MultExpression(MultExpression m, TermialCharacter<'*'>, Expression e)
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
        ConstructorParams<MultExpression, TermialCharacter<'*'>, Expression>,
        ConstructorParams<Expression>>;
    using ConstructorsNextSymbol =
        ConstructorTraits<TermialCharacter<'*'>, TermialCharacter<'+'>,
                          TermialCharacter<')'>>;
};

struct AddExpression {
    std::unique_ptr<AddExpression> a;
    std::unique_ptr<MultExpression> m;
    AddExpression(MultExpression m)
        : m(std::make_unique<MultExpression>(std::move(m))) {}
    AddExpression(AddExpression a, TermialCharacter<'+'>, MultExpression m)
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
        ConstructorParams<AddExpression, TermialCharacter<'+'>, MultExpression>,
        ConstructorParams<MultExpression>>;
    using ConstructorsNextSymbol =
        ConstructorTraits<TermialCharacter<'+'>, TermialCharacter<')'>>;
};