#pragma once

#include "parser/parser.h"
#include "tokens.h"
#include "variable.h"
#include <algorithm>
#include <charconv>
#include <map>
#include <optional>

struct Integer {
    int data;
    explicit Integer(IntegerToken str) {
        std::from_chars(str.str.data(), str.str.data() + str.str.size(), data);
    }
    friend std::ostream &operator<<(std::ostream &out, const Integer &d) {
        return out << "Integer(" << d.data << ")";
    }
};

template <> struct SymbolTraits<Integer> {
    using Constructors = ConstructorTraits<ConstructorParams<IntegerToken>>;
    using ConstructorsNextSymbol =
        ConstructorTraits<MultToken, AddToken, SubToken, CloseParenToken,
                          SemicolonToken, CommaToken, CloseSquareBraceToken>;
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

struct IndexIdentifier {
    Identifier i;
    std::unique_ptr<AddExpression> a;
};

struct Expression {
    std::variant<std::unique_ptr<AddExpression>, Identifier, Integer,
                 std::unique_ptr<FunctionCall>, IndexIdentifier,
                 StringLiteralToken>
        data;
    explicit Expression(Identifier i) : data(i) {}
    explicit Expression(Integer d) : data(d) {}
    explicit Expression(FunctionCall &&f)
        : data(std::make_unique<FunctionCall>(std::move(f))) {}
    explicit Expression(StringLiteralToken s) : data(s) {}
    Expression(OpenParenToken, AddExpression &&a, CloseParenToken)
        : data(std::make_unique<AddExpression>(std::move(a))) {}
    Expression(Identifier i, OpenSquareBraceToken, AddExpression &&a,
               CloseSquareBraceToken)
        : data(IndexIdentifier{i,
                               std::make_unique<AddExpression>(std::move(a))}) {
    }

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
        } else if (std::holds_alternative<std::unique_ptr<FunctionCall>>(
                       e.data)) {
            return out << "Expression("
                       << *std::get<std::unique_ptr<FunctionCall>>(e.data)
                       << ")";
        } else if (std::holds_alternative<IndexIdentifier>(e.data)) {
            auto &indexed = std::get<IndexIdentifier>(e.data);
            return out << "Expression(" << indexed.i << "[" << *indexed.a
                       << "])";
        } else {
            return out << "Expression(" << std::get<StringLiteralToken>(e.data)
                       << ")";
        }
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
        } else if (std::holds_alternative<std::unique_ptr<FunctionCall>>(
                       data)) {
            return evaluate_func_call(
                *std::get<std::unique_ptr<FunctionCall>>(data), variables);
        } else if (std::holds_alternative<IndexIdentifier>(data)) {
            auto &indexed = std::get<IndexIdentifier>(data);
            int index = std::get<int>(
                evaluate_add_expression(*indexed.a, variables).data);
            auto &var = variables.at(std::string{indexed.i.str});
            if (std::holds_alternative<std::vector<Variable>>(var.data)) {
                return std::get<std::vector<Variable>>(
                    var.data)[static_cast<std::vector<Variable>::size_type>(
                    index)];
            }
            return Variable{std::get<std::string>(
                var.data)[static_cast<std::string::size_type>(index)]};
        } else {
            auto string = std::get<StringLiteralToken>(data).str;
            string.remove_prefix(1);
            string.remove_suffix(1);
            return Variable{std::string{string}};
        }
    }
};

template <> struct SymbolTraits<Expression> {
    using Constructors = ConstructorTraits<
        ConstructorParams<Identifier>, ConstructorParams<Integer>,
        ConstructorParams<OpenParenToken, AddExpression, CloseParenToken>,
        ConstructorParams<FunctionCall>,
        ConstructorParams<Identifier, OpenSquareBraceToken, AddExpression,
                          CloseSquareBraceToken>,
        ConstructorParams<StringLiteralToken>>;
    using ConstructorsNextSymbol =
        ConstructorTraits<MultToken, AddToken, SubToken, CloseParenToken,
                          SemicolonToken, CommaToken, CloseSquareBraceToken>;
};

struct MultExpression {
    std::unique_ptr<MultExpression> m;
    Expression e;
    explicit MultExpression(Expression e) : e(std::move(e)) {}
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
        ConstructorTraits<MultToken, AddToken, SubToken, CloseParenToken,
                          SemicolonToken, CommaToken, CloseSquareBraceToken>;
};

struct AddExpression {
    std::unique_ptr<AddExpression> a;
    MultExpression m;
    bool is_add{true};
    explicit AddExpression(MultExpression m) : m(std::move(m)) {}
    AddExpression(AddExpression a, AddToken, MultExpression m)
        : a(std::make_unique<AddExpression>(std::move(a))), m(std::move(m)) {}
    AddExpression(AddExpression a, SubToken, MultExpression m)
        : a(std::make_unique<AddExpression>(std::move(a))), m(std::move(m)),
          is_add(false) {}

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
                if (is_add) {
                    return Variable{std::get<int>(a_val.data) +
                                    std::get<int>(m_val.data)};
                } else {
                    return Variable{std::get<int>(a_val.data) -
                                    std::get<int>(m_val.data)};
                }
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
        ConstructorParams<AddExpression, SubToken, MultExpression>,
        ConstructorParams<MultExpression>>;
    using ConstructorsNextSymbol =
        ConstructorTraits<AddToken, SubToken, CloseParenToken, SemicolonToken,
                          CommaToken, CloseSquareBraceToken>;
};

struct Assignment {
    Identifier i;
    AddExpression a;
    std::optional<AddExpression> indexExpr;
    Assignment(Identifier i, EqlToken, AddExpression a)
        : i(i), a(std::move(a)) {}
    Assignment(Identifier i, OpenSquareBraceToken, AddExpression indexExpr,
               CloseSquareBraceToken, EqlToken, AddExpression a)
        : i(i), a(std::move(a)), indexExpr(std::move(indexExpr)) {}

    friend std::ostream &operator<<(std::ostream &out,
                                    const Assignment &other) {
        return out << "Assignment(" << other.i << "=" << other.a << ")";
    }

    void evaluate(std::map<std::string, Variable> &variables) const {
        std::string key{i.str};
        if (indexExpr) {
            int index =
                std::get<int>(indexExpr.value().evaluate(variables).data);
            auto it = variables.find(key);
            if (it == variables.end()) {
                variables[key] = Variable{std::vector<Variable>(
                    static_cast<std::vector<Variable>::size_type>(index + 1))};
            }
            // TODO resize if out of range
            if (std::holds_alternative<std::vector<Variable>>(
                    variables.at(key).data)) {
                std::get<std::vector<Variable>>(
                    variables.at(key)
                        .data)[static_cast<std::vector<Variable>::size_type>(
                    index)] = a.evaluate(variables);
            } else {
                std::get<std::string>(
                    variables.at(key)
                        .data)[static_cast<std::string::size_type>(index)] =
                    static_cast<char>(
                        std::get<int>(a.evaluate(variables).data));
            }

        } else {
            variables[key] = a.evaluate(variables);
        }
    }
};

template <> struct SymbolTraits<Assignment> {
    using Constructors = ConstructorTraits<
        ConstructorParams<Identifier, EqlToken, AddExpression>,
        ConstructorParams<Identifier, OpenSquareBraceToken, AddExpression,
                          CloseSquareBraceToken, EqlToken, AddExpression>>;
    using ConstructorsNextSymbol = ConstructorTraits<SemicolonToken>;
};

struct FunctionParameters {
    Identifier i;
    std::vector<AddExpression> parameters;
    FunctionParameters(Identifier i) : i(i) {}
    FunctionParameters(Identifier i, OpenParenToken, AddExpression a) : i(i) {
        parameters.push_back(std::move(a));
    }
    FunctionParameters(FunctionParameters params, CommaToken, AddExpression a)
        : i(params.i), parameters{std::move(params.parameters)} {
        parameters.push_back(std::move(a));
    }

    friend std::ostream &operator<<(std::ostream &out,
                                    const FunctionParameters &other) {
        out << other.i << '(';
        for (const auto &param : other.parameters) {
            out << param << ',';
        }
        return out;
    }
};

template <> struct SymbolTraits<FunctionParameters> {
    using Constructors = ConstructorTraits<
        ConstructorParams<Identifier, OpenParenToken, AddExpression>,
        ConstructorParams<FunctionParameters, CommaToken, AddExpression>>;
    using ConstructorsNextSymbol =
        ConstructorTraits<CloseParenToken, CommaToken>;
};

struct FunctionCall {
    FunctionParameters params;
    FunctionCall(FunctionParameters params, CloseParenToken)
        : params(std::move(params)) {}
    FunctionCall(Identifier i, OpenParenToken, CloseParenToken) : params(i) {}

    friend std::ostream &operator<<(std::ostream &out,
                                    const FunctionCall &other) {
        return out << other.params << ")";
    }

    Variable evaluate(const std::map<std::string, Variable> &variables) const {
        auto fn_ptr = std::get<Variable (*)(const std::vector<Variable> &)>(
            variables.at(std::string{params.i.str}).data);
        std::vector<Variable> param_vals(params.parameters.size());
        std::transform(params.parameters.begin(), params.parameters.end(),
                       param_vals.begin(), [&](const auto &addExpr) {
                           return addExpr.evaluate(variables);
                       });
        return fn_ptr(param_vals);
    }
};

template <> struct SymbolTraits<FunctionCall> {
    using Constructors = ConstructorTraits<
        ConstructorParams<FunctionParameters, CloseParenToken>,
        ConstructorParams<Identifier, OpenParenToken, CloseParenToken>>;
    using ConstructorsNextSymbol =
        ConstructorTraits<MultToken, AddToken, SubToken, CloseParenToken,
                          SemicolonToken, CommaToken>;
};

Variable evaluate_func_call(const FunctionCall &f,
                            const std::map<std::string, Variable> &variables) {
    return f.evaluate(variables);
}