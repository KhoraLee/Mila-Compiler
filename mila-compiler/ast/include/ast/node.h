#pragma once

#include <string>

struct SourceLocation {
    int line;
    int column;
    SourceLocation(int l, int c) : line(l), column(c) {}
};

class ASTVisitor;

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor& visitor) = 0;
    SourceLocation location;
    ASTNode(int line, int column) : location(line, column) {}
};
