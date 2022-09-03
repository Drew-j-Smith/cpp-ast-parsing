#pragma once

#include "parser.h"
#include <map>

template <char terminal> struct TermialCharacter {};

template <typename Variant, char terminal>
struct TerminalTraits<TermialCharacter<terminal>, Variant> {
    static ParseResult<Variant> shift(std::string_view str) {
        if (str[0] == terminal) {
            return {TermialCharacter<terminal>{},
                    std::string_view{str.data() + 1, str.size() - 1}};
        }
        return {};
    }
};

template <char c>
std::ostream &operator<<(std::ostream &out, TermialCharacter<c>) {
    return out << c;
}

struct Identifier {
    std::string_view str;
    friend std::ostream &operator<<(std::ostream &out, const Identifier &i) {
        return out << "Identifier(" << i.str << ")";
    }
};

template <typename Variant> struct TerminalTraits<Identifier, Variant> {
    static ParseResult<Variant> shift(std::string_view str) {
        std::size_t index = 0;
        while (index < str.size() && str[index] != '+' && str[index] != '*' &&
               str[index] != '(' && str[index] != ')' && str[index] != '=') {
            ++index;
        }
        if (index == 0) {
            return {};
        }
        return {Identifier{{str.data(), index}},
                std::string_view{str.data() + index, str.size() - index}};
    }
};

struct AddExpression;
std::ostream &operator<<(std::ostream &out, const AddExpression &a);
double evaluate_add_expression(const AddExpression &a,
                               const std::map<std::string, double> &variables);

struct Expression {
    std::variant<std::unique_ptr<AddExpression>, Identifier> data;
    explicit Expression(Identifier i) : data(i) {}
    Expression(TermialCharacter<'('>, AddExpression &&a, TermialCharacter<')'>)
        : data(std::make_unique<AddExpression>(std::move(a))) {}
    friend std::ostream &operator<<(std::ostream &out, const Expression &e) {
        if (std::holds_alternative<Identifier>(e.data)) {
            return out << "Expression(" << std::get<Identifier>(e.data).str
                       << ")";
        }
        return out << "Expression("
                   << *std::get<std::unique_ptr<AddExpression>>(e.data) << ")";
    }
    double evaluate(const std::map<std::string, double> &variables) const {
        if (std::holds_alternative<Identifier>(data)) {
            return variables.at(std::string{std::get<Identifier>(data).str});
        } else {
            return evaluate_add_expression(
                *std::get<std::unique_ptr<AddExpression>>(data), variables);
        }
    }
};

template <> struct SymbolTraits<Expression> {
    using Constructors = ConstructorTraits<
        ConstructorParams<Identifier>,
        ConstructorParams<TermialCharacter<'('>, AddExpression,
                          TermialCharacter<')'>>>;
    using ConstructorsNextSymbol = ConstructorTraits<
        ConstructorParams<TermialCharacter<'*'>, TermialCharacter<'+'>,
                          TermialCharacter<')'>>,
        ConstructorParams<TermialCharacter<'*'>, TermialCharacter<'+'>>>;
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
    using ConstructorsNextSymbol = ConstructorTraits<
        ConstructorParams<TermialCharacter<'*'>, TermialCharacter<'+'>,
                          TermialCharacter<')'>>,
        ConstructorParams<TermialCharacter<'*'>, TermialCharacter<'+'>,
                          TermialCharacter<')'>>>;
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
    using ConstructorsNextSymbol = ConstructorTraits<
        ConstructorParams<TermialCharacter<'+'>, TermialCharacter<')'>>,
        ConstructorParams<TermialCharacter<'+'>, TermialCharacter<')'>>>;
};