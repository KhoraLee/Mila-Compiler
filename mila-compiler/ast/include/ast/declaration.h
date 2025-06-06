#pragma once

#include <ast/node.h>
#include <ast/visitor.h>

class Declaration : public ASTNode {
public:
    virtual ~Declaration() = default;
    virtual void accept(ASTVisitor& visitor) override {
        visitor.visitDeclaration(this);
    }
};
