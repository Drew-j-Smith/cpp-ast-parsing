# smithscript

This is my first attempt at creating a programming language. The goal is to create an interpred language in C++.

The only dependency is [ccompile-time-regular-expressions](https://github.com/hanickadot/compile-time-regular-expressions)

This project relies heavily on templates on constexpr and some c++20 features. It will not work on older compilers or on msvc (due to initializer list not executing in order).

## Grammar

I will be excluding the terminal symbols. They are defined by the the regular expressions in src/script/tokens.h

```
      <FunctionCall> ::= <FunctionParameters><CloseParenToken> 
                         | <Identifier><OpenParenToken><CloseParenToken>
      <IfExpression> ::= <IfCondition><OpenBraceToken><Block><CloseBraceToken>
       <IfCondition> ::= <IfToken><OpenParenToken><AddExpression><CloseParenToken>
   <WhileExpression> ::= <WhileCondition><OpenBraceToken><Block><CloseBraceToken>
    <WhileCondition> ::= <WhileToken><OpenParenToken><AddExpression><CloseParenToken>
           <Integer> ::= <IntegerToken>
        <Expression> ::= <Identifier> 
                         | <Integer> 
                         | <OpenParenToken><AddExpression><CloseParenToken> 
                         | <FunctionCall> 
                         | <Identifier><OpenSquareBraceToken><AddExpression><CloseSquareBraceToken>
    <MultExpression> ::= <MultExpression><MultToken><Expression> 
                         | <Expression>
     <AddExpression> ::= <AddExpression><AddToken><MultExpression> 
                         | <AddExpression><SubToken><MultExpression> 
                         | <MultExpression>
        <Assignment> ::= <Identifier><EqlToken><AddExpression> 
                         | <Identifier><OpenSquareBraceToken><AddExpression><CloseSquareBraceToken><EqlToken><AddExpression>
         <Statement> ::= <AddExpression><SemicolonToken> 
                         | <Assignment><SemicolonToken> 
                         | <IfExpression> 
                         | <WhileExpression>
             <Block> ::= <Block><Statement> | <Statement>
<FunctionParameters> ::= <Identifier><OpenParenToken><AddExpression> 
                         | <FunctionParameters><CommaToken><AddExpression>
```