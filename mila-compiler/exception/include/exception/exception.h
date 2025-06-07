#pragma once

#include <lexer/types.h>

#include <iostream>
#include <string>
#include <sstream>

class MilaException : public std::runtime_error {
protected:
  MilaException(SourceLocation loc, const std::string& message) : std::runtime_error(format_message(loc, message)), loc(loc) {}
  SourceLocation loc;

private:
  static std::string format_message(const SourceLocation loc, const std::string msg) {
    std::stringstream message;
    message << "Error at line " << loc.line << ", column " << loc.column << ": " << msg;
    return message.str();
  }
};
