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
    Statement(SourceLocation loc) : ASTNode(std::move(loc)) {}
};

using Stmt = std::shared_ptr<Statement>;

class AssignStmt : public Statement {
public:
    AssignStmt(std::string target, Expr value, SourceLocation loc)
        : Statement(std::move(loc)), _target(std::move(target)), _value(std::move(value)) {}
    const std::string& target() const { return _target; }
    const Expr& value() const { return _value; }
private:
    std::string _target;
    Expr _value;
};

using Assign_S = std::shared_ptr<AssignStmt>;

class ArrayAssignStmt : public AssignStmt {
public:
    ArrayAssignStmt(std::string array, Expr index, Expr value, SourceLocation loc)
        : AssignStmt(std::move(array), std::move(value), std::move(loc)), _index(std::move(index)) {}
    const Expr& index() const { return _index; }
private:
    Expr _index;
};

using ArrayAssign_S = std::shared_ptr<ArrayAssignStmt>;

class IfStmt : public Statement {
public:
    IfStmt(Expr condition, Stmt thenBranch, Stmt elseBranch, SourceLocation loc)
        : Statement(std::move(loc)), _condition(std::move(condition)), _thenBranch(std::move(thenBranch)), _elseBranch(std::move(elseBranch)) {}
    const Expr& condition() const { return _condition; }
    const Stmt& thenBranch() const { return _thenBranch; }
    const Stmt& elseBranch() const { return _elseBranch; }
private:
    Expr _condition;
    Stmt _thenBranch;
    Stmt _elseBranch;
};

using If_S = std::shared_ptr<IfStmt>;

class WhileStmt : public Statement {
public:
    WhileStmt(Expr condition, Stmt body, SourceLocation loc)
        : Statement(std::move(loc)), _condition(std::move(condition)), _body(std::move(body)) {}
    const Expr& condition() const { return _condition; }
    const Stmt& body() const { return _body; }
private:
    Expr _condition;
    Stmt _body;
};

using While_S = std::shared_ptr<WhileStmt>;

class ForStmt : public Statement {
public:
    ForStmt(std::string loopVar, Expr start, Expr end, Stmt body, bool downto, SourceLocation loc)
        : Statement(std::move(loc)), _loopVar(std::move(loopVar)), _start(std::move(start)), _end(std::move(end)), _body(std::move(body)), _downto(downto) {}
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

using For_S = std::shared_ptr<ForStmt>;

class BlockStmt : public Statement {
public:
    BlockStmt(std::vector<Stmt> statements, SourceLocation loc)
        : Statement(std::move(loc)), _statements(std::move(statements)) {}
    const std::vector<Stmt>& statements() const { return _statements; }
private:
    std::vector<Stmt> _statements;
};

using Block_S = std::shared_ptr<BlockStmt>;

class CallStmt : public Statement {
public:
    CallStmt(std::string callee, std::vector<Expr> args, SourceLocation loc)
        : Statement(std::move(loc)), _callee(std::move(callee)), _args(std::move(args)) {}
    const std::string& callee() const { return _callee; }
    const std::vector<Expr>& args() const { return _args; }
private:
    std::string _callee;
    std::vector<Expr> _args;
};

using Call_S = std::shared_ptr<CallStmt>;

class BreakStmt : public Statement {
public:
    BreakStmt(SourceLocation loc) : Statement(std::move(loc)) {}
};

using Break_S = std::shared_ptr<BreakStmt>;

class ExitStmt : public Statement {
public:
    ExitStmt(SourceLocation loc) : Statement(std::move(loc)) {}
};

using Exit_S = std::shared_ptr<ExitStmt>;
