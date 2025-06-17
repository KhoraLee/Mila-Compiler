#pragma once

class IntegerLiteral;
class FloatLiteral; 
class StringLiteral;
class VariableExpr;
class BinaryExpr; 
class UnaryExpr; 
class CallExpr; 
class ParenExpr; 
class ArrayAccess; 

class AssignStmt;
class ArrayAssignStmt;
class IfStmt;
class WhileStmt;
class ForStmt;
class BlockStmt;
class CallStmt;
class BreakStmt;
class ExitStmt;

class NamedDecl;
class ConstDecl;
class VarDecl;
class ArrayDecl;
class FunctionDecl;
class ProgramDecl;

class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;

    // Expression
    virtual void visit(IntegerLiteral* expr) = 0;
    virtual void visit(FloatLiteral* expr) = 0;
    virtual void visit(StringLiteral* expr) = 0;
    virtual void visit(VariableExpr* expr) = 0;
    virtual void visit(BinaryExpr* expr) = 0;
    virtual void visit(UnaryExpr* expr) = 0;
    virtual void visit(CallExpr* expr) = 0;
    virtual void visit(ParenExpr* expr) = 0;
    virtual void visit(ArrayAccess* expr) = 0;

    // Statement
    virtual void visit(AssignStmt* stmt) = 0;
    virtual void visit(ArrayAssignStmt* stmt) = 0;
    virtual void visit(IfStmt* stmt) = 0;
    virtual void visit(WhileStmt* stmt) = 0;
    virtual void visit(ForStmt* stmt) = 0;
    virtual void visit(BlockStmt* stmt) = 0;
    virtual void visit(CallStmt* stmt) = 0;
    virtual void visit(BreakStmt* stmt) = 0;
    virtual void visit(ExitStmt* stmt) = 0;

    // Declaration
    virtual void visit(NamedDecl* decl) = 0;
    virtual void visit(ConstDecl* decl) = 0;
    virtual void visit(VarDecl* decl) = 0;
    virtual void visit(ArrayDecl* decl) = 0;
    virtual void visit(FunctionDecl* decl) = 0;
    virtual void visit(ProgramDecl* decl) = 0;
};
