#pragma once

#include <ast/type.h>
#include <ast/node.h>
#include <ast/visitor.h>
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

class FloatLiteral : public Expression {
public:
    FloatLiteral(double value, SourceLocation loc) : Expression(std::move(loc)), _value(value) {}
    double value() const { return _value; }
private:
    double _value;
};

class StringLiteral : public Expression {
public:
    StringLiteral(std::string value, SourceLocation loc) : Expression(std::move(loc)), _value(std::move(value)) {}
    const std::string& value() const { return _value; }
private:
    std::string _value;
};

class VariableExpr : public Expression {
public:
    VariableExpr(std::string name, SourceLocation loc) : Expression(std::move(loc)), _name(std::move(name)) {}
    const std::string& name() const { return _name; }
private:
    std::string _name;
};

class BinaryExpr : public Expression {
public:
    BinaryExpr(Expr left, Expr right, std::string op, SourceLocation loc)
        : Expression(std::move(loc)), _left(std::move(left)), _right(std::move(right)), _op(std::move(op)) {}
    const Expr& left() const { return _left; }
    const Expr& right() const { return _right; }
    const std::string& op() const { return _op; }
private:
    Expr _left;
    Expr _right;
    std::string _op;
};

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

class ParenExpr : public Expression {
public:
    ParenExpr(Expr expr, SourceLocation loc)
        : Expression(std::move(loc)), _expr(std::move(expr)) {}
    const Expr& expr() const { return _expr; }
private:
    Expr _expr;
};

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
