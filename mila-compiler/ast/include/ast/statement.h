#pragma once

#include <ast/node.h>
#include <ast/visitor.h>
#include <ast/expression.h>
#include <string>
#include <memory>
#include <vector>

class Statement : public ASTNode {
public:
    virtual ~Statement() = default;
    virtual void accept(ASTVisitor& visitor) override {
        visitor.visitStatement(this);
    }
    Statement(int line, int column) : ASTNode(line, column) {}
};

typedef std::shared_ptr<Statement> Stmt;

class AssignStmt : public Statement {
public:
    AssignStmt(std::string target, Expr value, int line, int column)
        : Statement(line, column), _target(std::move(target)), _value(std::move(value)) {}
    const std::string& target() const { return _target; }
    const Expr& value() const { return _value; }
private:
    std::string _target;
    Expr _value;
};

class IfStmt : public Statement {
public:
    IfStmt(Expr condition, Stmt thenBranch, Stmt elseBranch, int line, int column)
        : Statement(line, column), _condition(std::move(condition)), _thenBranch(std::move(thenBranch)), _elseBranch(std::move(elseBranch)) {}
    const Expr& condition() const { return _condition; }
    const Stmt& thenBranch() const { return _thenBranch; }
    const Stmt& elseBranch() const { return _elseBranch; }
private:
    Expr _condition;
    Stmt _thenBranch;
    Stmt _elseBranch;
};

class WhileStmt : public Statement {
public:
    WhileStmt(Expr condition, Stmt body, int line, int column)
        : Statement(line, column), _condition(std::move(condition)), _body(std::move(body)) {}
    const Expr& condition() const { return _condition; }
    const Stmt& body() const { return _body; }
private:
    Expr _condition;
    Stmt _body;
};

class ForStmt : public Statement {
public:
    ForStmt(std::string loopVar, Expr start, Expr end, Stmt body, bool downto, int line, int column)
        : Statement(line, column), _loopVar(std::move(loopVar)), _start(std::move(start)), _end(std::move(end)), _body(std::move(body)), _downto(downto) {}
    const std::string& loopVar() const { return _loopVar; }
    const Expr& start() const { return _start; }
    const Expr& end() const { return _end; }
    const Stmt& body() const { return _body; }
    bool downto() const { return _downto; }
private:
    std::string _loopVar;
    Expr _start;
    Expr _end;
    Stmt _body;
    bool _downto;
};

class BlockStmt : public Statement {
public:
    BlockStmt(std::vector<Stmt> statements, int line, int column)
        : Statement(line, column), _statements(std::move(statements)) {}
    const std::vector<Stmt>& statements() const { return _statements; }
private:
    std::vector<Stmt> _statements;
};

class CallStmt : public Statement {
public:
    CallStmt(std::string callee, std::vector<Expr> args, int line, int column)
        : Statement(line, column), _callee(std::move(callee)), _args(std::move(args)) {}
    const std::string& callee() const { return _callee; }
    const std::vector<Expr>& args() const { return _args; }
private:
    std::string _callee;
    std::vector<Expr> _args;
};

// TODO: Is it necessary?
class BreakStmt : public Statement {
public:
    BreakStmt(int line, int column) : Statement(line, column) {}
};

class ExitStmt : public Statement {
public:
    ExitStmt(int line, int column) : Statement(line, column) {}
};
