#pragma once

#include <string>
#include <lexer/types.h>

struct SourceLocation {
    int line;
    int column;
    SourceLocation(int l, int c) : line(l), column(c) {}
};

struct Param {
    std::string name;
    TokenType type;
};

using ParamList = std::vector<Param>;
