#pragma once

#include <ast/node.h>
#include <ast/visitor.h>

class Expression : public ASTNode {
public:
    virtual ~Expression() = default;
    virtual void accept(ASTVisitor& visitor) override {
        visitor.visitExpression(this);
    }
    Expression(int line, int column) : ASTNode(line, column) {}
};
