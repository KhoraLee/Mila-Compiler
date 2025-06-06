#pragma once

#include <ast/types.h>
#include <ast/node.h>
#include <ast/visitor.h>
#include <lexer/token.h>
#include <string>
#include <memory>
#include <vector>

class Expression : public ASTNode {
public:
    virtual ~Expression() = default;
    virtual void accept(ASTVisitor& visitor) override {
        visitor.visitExpression(this);
    }
    Expression(SourceLocation loc) : ASTNode(std::move(loc)) {}
};

using Expr = std::shared_ptr<Expression>;

class IntegerLiteral : public Expression {
public:
    IntegerLiteral(int value, SourceLocation loc) : Expression(std::move(loc)), _value(value) {}
    int value() const { return _value; }
private:
    int _value;
};

using Integer_E = std::shared_ptr<IntegerLiteral>;

class FloatLiteral : public Expression {
public:
    FloatLiteral(double value, SourceLocation loc) : Expression(std::move(loc)), _value(value) {}
    double value() const { return _value; }
private:
    double _value;
};

using Float_E = std::shared_ptr<FloatLiteral>;

class StringLiteral : public Expression {
public:
    StringLiteral(std::string value, SourceLocation loc) : Expression(std::move(loc)), _value(std::move(value)) {}
    const std::string& value() const { return _value; }
private:
    std::string _value;
};

using String_E = std::shared_ptr<StringLiteral>;

class VariableExpr : public Expression {
public:
    VariableExpr(std::string name, SourceLocation loc) : Expression(std::move(loc)), _name(std::move(name)) {}
    const std::string& name() const { return _name; }
private:
    std::string _name;
};

using Variable_E = std::shared_ptr<VariableExpr>;

class BinaryExpr : public Expression {
public:
    BinaryExpr(Expr left, Expr right, std::shared_ptr<OperatorToken> op, SourceLocation loc)
        : Expression(std::move(loc)), _left(std::move(left)), _right(std::move(right)), _op(std::move(op)) {}
    const Expr& left() const { return _left; }
    const Expr& right() const { return _right; }
    const std::shared_ptr<OperatorToken>& op() const { return _op; }
private:
    Expr _left;
    Expr _right;
    std::shared_ptr<OperatorToken> _op;
};

using Binary_E = std::shared_ptr<BinaryExpr>;

class UnaryExpr : public Expression {
public:
    UnaryExpr(std::shared_ptr<OperatorToken> op, Expr expr, SourceLocation loc)
        : Expression(std::move(loc)), _op(std::move(op)), _expr(std::move(expr)) {}
    const Expr& expr() const { return _expr; }
    const std::shared_ptr<OperatorToken>& op() const { return _op; }
private:
    std::shared_ptr<OperatorToken> _op;
    Expr _expr;
};

using Unary_E = std::shared_ptr<UnaryExpr>;

class CallExpr : public Expression {
public:
    CallExpr(std::string callee, std::vector<Expr> args, SourceLocation loc)
        : Expression(std::move(loc)), _callee(std::move(callee)), _args(std::move(args)) {}
    const std::string& callee() const { return _callee; }
    const std::vector<Expr>& args() const { return _args; }
private:
    std::string _callee;
    std::vector<Expr> _args;
};

using Call_E = std::shared_ptr<CallExpr>;

class ParenExpr : public Expression {
public:
    ParenExpr(Expr expr, SourceLocation loc)
        : Expression(std::move(loc)), _expr(std::move(expr)) {}
    const Expr& expr() const { return _expr; }
private:
    Expr _expr;
};

using Paren_E = std::shared_ptr<ParenExpr>;

class ArrayAccess : public Expression {
public:
    ArrayAccess(Expr array, Expr index, SourceLocation loc)
        : Expression(std::move(loc)), _array(std::move(array)), _index(std::move(index)) {}
    const Expr& array() const { return _array; }
    const Expr& index() const { return _index; }
private:
    Expr _array;
    Expr _index;
};

using ArrayAccess_E = std::shared_ptr<ArrayAccess>;
