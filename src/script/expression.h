#pragma once

#include "parser/parser.h"
#include "tokens.h"
#include "variable.h"
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
        ConstructorTraits<MultToken, AddToken, CloseParenToken, SemicolonToken>;
};

struct FunctionCall;
struct AddExpression;
std::ostream &operator<<(std::ostream &out, const AddExpression &a);
std::ostream &operator<<(std::ostream &out, const FunctionCall &a);
Variable
evaluate_add_expression(const AddExpression &a,
                        const std::map<std::string, Variable> &variables);
Variable evaluate_func_call(const FunctionCall &f,
                            const std::map<std::string, Variable> &variables);

struct Expression {
    std::variant<std::unique_ptr<AddExpression>, Identifier, Integer,
                 std::unique_ptr<FunctionCall>>
        data;
    explicit Expression(Identifier i) : data(i) {}
    explicit Expression(Integer d) : data(d) {}
    explicit Expression(FunctionCall &&f)
        : data(std::make_unique<FunctionCall>(std::move(f))) {}
    Expression(OpenParenToken, AddExpression &&a, CloseParenToken)
        : data(std::make_unique<AddExpression>(std::move(a))) {}
    friend std::ostream &operator<<(std::ostream &out, const Expression &e) {
        if (std::holds_alternative<Identifier>(e.data)) {
            return out << "Expression(" << std::get<Identifier>(e.data).str
                       << ")";
        } else if (std::holds_alternative<Integer>(e.data)) {
            return out << "Expression(" << std::get<Integer>(e.data).data
                       << ")";
        } else if (std::holds_alternative<std::unique_ptr<AddExpression>>(
                       e.data)) {
            return out << "Expression("
                       << *std::get<std::unique_ptr<AddExpression>>(e.data)
                       << ")";
        }
        return out << "FunctionCall("
                   << *std::get<std::unique_ptr<FunctionCall>>(e.data) << ")";
    }
    Variable evaluate(const std::map<std::string, Variable> &variables) const {
        if (std::holds_alternative<Identifier>(data)) {
            return variables.at(std::string{std::get<Identifier>(data).str});
        } else if (std::holds_alternative<Integer>(data)) {
            return Variable{std::get<Integer>(data).data};
        } else if (std::holds_alternative<std::unique_ptr<AddExpression>>(
                       data)) {
            return evaluate_add_expression(
                *std::get<std::unique_ptr<AddExpression>>(data), variables);
        } else {
            return evaluate_func_call(
                *std::get<std::unique_ptr<FunctionCall>>(data), variables);
        }
    }
};

template <> struct SymbolTraits<Expression> {
    using Constructors = ConstructorTraits<
        ConstructorParams<Identifier>, ConstructorParams<Integer>,
        ConstructorParams<OpenParenToken, AddExpression, CloseParenToken>,
        ConstructorParams<FunctionCall>>;
    using ConstructorsNextSymbol =
        ConstructorTraits<MultToken, AddToken, CloseParenToken, SemicolonToken>;
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

    Variable evaluate(const std::map<std::string, Variable> &variables) const {
        if (m) {
            auto m_val = m->evaluate(variables);
            auto e_val = e.evaluate(variables);
            if (std::holds_alternative<int>(m_val.data) &&
                std::holds_alternative<int>(e_val.data)) {
                return Variable{std::get<int>(m_val.data) *
                                std::get<int>(e_val.data)};
            }
            throw std::runtime_error{"Invalid mult types"};
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
        ConstructorTraits<MultToken, AddToken, CloseParenToken, SemicolonToken>;
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
    Variable evaluate(const std::map<std::string, Variable> &variables) const {
        if (a) {
            auto a_val = a->evaluate(variables);
            auto m_val = m.evaluate(variables);
            if (std::holds_alternative<int>(a_val.data) &&
                std::holds_alternative<int>(m_val.data)) {
                return Variable{std::get<int>(a_val.data) *
                                std::get<int>(m_val.data)};
            }
            throw std::runtime_error{"Invalid add types"};
        } else {
            return m.evaluate(variables);
        }
    }
};

Variable
evaluate_add_expression(const AddExpression &a,
                        const std::map<std::string, Variable> &variables) {
    return a.evaluate(variables);
}

template <> struct SymbolTraits<AddExpression> {
    using Constructors = ConstructorTraits<
        ConstructorParams<AddExpression, AddToken, MultExpression>,
        ConstructorParams<MultExpression>>;
    using ConstructorsNextSymbol =
        ConstructorTraits<AddToken, CloseParenToken, SemicolonToken>;
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

    void evaluate(std::map<std::string, Variable> &variables) const {
        variables[std::string{i.str}] = a.evaluate(variables);
    }
};

template <> struct SymbolTraits<Assignment> {
    using Constructors = ConstructorTraits<
        ConstructorParams<Identifier, EqlToken, AddExpression>>;
    using ConstructorsNextSymbol = ConstructorTraits<SemicolonToken>;
};

struct FunctionCall {
    Identifier i;
    AddExpression a;
    FunctionCall(Identifier i, OpenParenToken, AddExpression a, CloseParenToken)
        : i(i), a(std::move(a)) {}

    friend std::ostream &operator<<(std::ostream &out,
                                    const FunctionCall &other) {
        return out << other.i << "(" << other.a << ")";
    }

    Variable evaluate(const std::map<std::string, Variable> &variables) const {
        auto fn_ptr = std::get<Variable (*)(Variable)>(
            variables.at(std::string{i.str}).data);
        return fn_ptr(a.evaluate(variables));
    }
};

template <> struct SymbolTraits<FunctionCall> {
    using Constructors =
        ConstructorTraits<ConstructorParams<Identifier, OpenParenToken,
                                            AddExpression, CloseParenToken>>;
    using ConstructorsNextSymbol =
        ConstructorTraits<MultToken, AddToken, CloseParenToken, SemicolonToken>;
};

Variable evaluate_func_call(const FunctionCall &f,
                            const std::map<std::string, Variable> &variables) {
    return f.evaluate(variables);
}