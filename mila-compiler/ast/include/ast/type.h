#pragma once

#include <string>
#include <lexer/types.h>

struct Param {
    std::string name;
    TokenType type;
};

using ParamList = std::vector<Param>;
