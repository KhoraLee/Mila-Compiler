#pragma once

#include <exception/exception.h>

class CodeGenException : public MilaException {
public:
  CodeGenException(SourceLocation loc, const std::string& message) : MilaException(loc, message) {}
};
