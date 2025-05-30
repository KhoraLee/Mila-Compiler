#pragma once

#include<lexer/token.h>
#include<lexer/types.h>

#include <iostream>
#include <string>

class Lexer {
public:
  explicit Lexer(std::istream& input);
  ~Lexer() = default;
  std::shared_ptr<Token> next_token();

private:
  std::istream& stream;
  char current;
  std::shared_ptr<Token> prev_token;

  char read_char();
  double read_num(bool& is_float);
  int read_num(IntBase base);
  std::string read_string();
  std::string read_identifier();
  std::string read_operator();

  const std::set<char> space_set = { ' ', '\t', '\n' };
  const std::set<char> op_set = { '+', '-', '*', '/' , '<', '>', '=', ':' };
};
