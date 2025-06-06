#pragma once

#include <ast/node.h>

class Statement : public ASTNode {
public:
    virtual ~Statement() = default;
};
