#pragma once

#include <ast/node.h>

class Expression : public ASTNode {
public:
    virtual ~Expression() = default;
};
