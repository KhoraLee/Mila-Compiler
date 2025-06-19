#pragma once

#include <exception/exception.h>

class LexerException : public MilaException {
public:
  using MilaException::MilaException;
};

class InvalidSymbolException : public LexerException {
public:
  InvalidSymbolException(SourceLocation loc, char symbol)
    : LexerException(loc, format_message(symbol)) {}

private:
  static std::string format_message(char symbol) {
    switch (symbol) {
      case '\n': return "Invalid symbol: '\\n'";
      case '\t': return "Invalid symbol: '\\t'";
      case '\0': return "Invalid symbol: '\\0'";
      default: return "Invalid symbol: '" + std::string(1, symbol) + "'";
    }
  }
};

class InvalidNumberException : public LexerException {
public:
  InvalidNumberException(SourceLocation loc) : LexerException(loc, "invalid number literal") {}
};

class UnterminatedStringException : public LexerException {
public:
  UnterminatedStringException(SourceLocation loc) : LexerException(loc, "unterminated string literal") {}
};
