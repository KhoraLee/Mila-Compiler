#pragma once

#include <ast/type.h>

class ASTVisitor;

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor& visitor) = 0;
    SourceLocation location;
    ASTNode(int line, int column) : location(line, column) {}
};
