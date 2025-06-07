#pragma once

#include <ast/node.h>
#include <ast/expression.h>
#include <ast/statement.h>
#include <ast/declaration.h>
#include <exception/parser_exception.h>
#include <lexer/lexer.h>
#include <lexer/token.h>

#include <memory>
#include <string>

class Parser {
public:
    explicit Parser(Lexer& lexer);
    Program_D parse();

private:
    // Token management
    void advance();
    bool check(TokenType type) const;
    bool match(TokenType type);
    void consume(TokenType type, const std::string& message);
    bool isAtEnd() const;

    // Expression
    Expr expression();
    Expr simpleExpression();
    Expr term();
    Expr factor();
    Expr primary();
    Expr variableOrCall();

    // Statement
    Stmt statement();
    Assign_S assignment(const std::string& target);
    If_S ifStatement();
    While_S whileStatement();
    For_S forStatement();
    Block_S blockStatement();
    Call_S callStatement(const std::string& callee);
    Break_S breakStatement();
    Exit_S exitStatement();

    // Declaration
    Decl declaration();
    Const_D constDeclaration();
    Var_D varDeclaration();
    Array_D arrayDeclaration();
    Function_D functionDeclaration();
    Program_D programDeclaration();

    Lexer& _lexer;
    TokenPtr _current;
    TokenPtr _previous;
};
