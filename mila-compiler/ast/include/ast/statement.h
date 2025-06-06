#pragma once

#include <ast/node.h>
#include <ast/visitor.h>

class Statement : public ASTNode {
public:
    virtual ~Statement() = default;
    virtual void accept(ASTVisitor& visitor) override {
        visitor.visitStatement(this);
    }
    Statement(int line, int column) : ASTNode(line, column) {}
};
