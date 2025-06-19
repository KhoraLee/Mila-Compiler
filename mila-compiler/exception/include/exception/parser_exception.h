#pragma once

#include <exception/exception.h>

class ParserException : public MilaException {
public:
  using MilaException::MilaException;
};

class UnexpectedTokenException : public ParserException {
public:
  UnexpectedTokenException(SourceLocation loc, const std::string& message) : ParserException(loc, message) {}
};

class SyntaxErrorException : public ParserException {
public:
  SyntaxErrorException(SourceLocation loc, const std::string& message) : ParserException(loc, message) {}
};

class MissingTokenException : public ParserException {
public:
  MissingTokenException(SourceLocation loc, const std::string& message) : ParserException(loc, message) {}
};

class UnmatchedParenthesisException : public ParserException {
public:
  UnmatchedParenthesisException(SourceLocation loc, const std::string& message) : ParserException(loc, message) {}
};
