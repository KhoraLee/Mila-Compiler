#pragma once

#include <ast/types.h>

class ASTVisitor;

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor& visitor) = 0;
    SourceLocation location() const { return loc; }
protected:
    SourceLocation loc;
    ASTNode(SourceLocation loc) : loc(std::move(loc)) {}
};
