#pragma once

#include <lexer/types.h>

#include <string> 

class Token {
public:
  virtual ~Token() = default;
  virtual TokenType type() const = 0;
  SourceLocation location() const { return loc; }
  // virtual std::string to_string() const = 0;
  virtual int precedence() const { return -1; }

protected:
  Token(SourceLocation loc) : loc(loc) {}
  SourceLocation loc;
};

using TokenPtr = std::shared_ptr<Token>;

class BasicToken : public Token {
public:
  BasicToken(const TokenType type, SourceLocation loc) : Token(loc), _type(type) {}
  TokenType type() const override { return _type; }

private:
  TokenType _type;
};

class OperatorToken : public Token {
  public:
  OperatorToken(const TokenType type, SourceLocation loc) : Token(loc), _type(type) {}
  TokenType type() const override { return _type; }
  int precedence() const override;

private:
  TokenType _type;
};

class IdentifierToken : public Token {
public:
  IdentifierToken(const std::string& name, SourceLocation loc) : Token(loc), _name(name) {}
  TokenType type() const override { return TOK_IDENTIFIER; }
  std::string name() const { return _name; }

private:
  std::string _name;
};

class IntToken : public Token {
public:
  IntToken(const int value, SourceLocation loc) : Token(loc), _val(value) {}
  TokenType type() const override { return TOK_INTEGER; }
  int value() const { return _val; }

private:
  int _val;
};

class FloatToken : public Token {
public:
  FloatToken(const double value, SourceLocation loc) : Token(loc), _val(value) {}
  TokenType type() const override { return TOK_FLOAT; }
  int value() const { return _val; }

private:
  double _val;
};

class StringToken : public Token {
public:
  StringToken(std::string str, SourceLocation loc) : Token(loc), _string(std::move(str)) {}
  TokenType type() const override { return TOK_STRING; }
  std::string string() const { return _string; }

private:
  std::string _string;
};
