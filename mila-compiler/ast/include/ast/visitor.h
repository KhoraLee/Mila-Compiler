#pragma once

class Expression;
class Statement;
class Declaration;

class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    virtual void visitExpression(Expression* expr) = 0;
    virtual void visitStatement(Statement* stmt) = 0;
    virtual void visitDeclaration(Declaration* decl) = 0;
};
