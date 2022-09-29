

#include "script/script.h"

template <typename... Params>
void print_ctor_grammar(std::ostream &out, ConstructorParams<Params...>) {
    ([&] { out << "<" << typeid(Params).name() << ">"; }(), ...);
}

template <typename Symbol, typename... Ctors>
void print_ctors_grammar(std::ostream &out, ConstructorTraits<Ctors...>) {
    out << "<" << typeid(Symbol).name() << "> ::= ";
    (
        [&] {
            print_ctor_grammar(out, Ctors{});
            out << " |";
        }(),
        ...);
    out << '\n';
}

template <typename Symbol> void print_symbol_grammar(std::ostream &out) {
    print_ctors_grammar<Symbol>(out,
                                typename SymbolTraits<Symbol>::Constructors{});
}

template <typename... SymbolArgs> void print_grammar(std::ostream &out) {
    (print_symbol_grammar<SymbolArgs>(out), ...);
}

int main() {

    print_grammar<FunctionCall, IfExpression, IfCondition, WhileExpression,
                  WhileCondition, Integer, Expression, MultExpression,
                  AddExpression, Assignment, Statement, Block,
                  FunctionParameters>(std::cout);
}