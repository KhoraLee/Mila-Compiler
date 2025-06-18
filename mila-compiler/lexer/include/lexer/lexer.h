#pragma once

#include<lexer/token.h>
#include<lexer/types.h>

#include <iostream>
#include <string>

class Lexer {
public:
  explicit Lexer(std::istream& input);
  ~Lexer() = default;
  TokenPtr next_token();

private:
  std::istream& stream;
  char current;
  TokenPtr prev_token;

  char read_char();
  void unget();
  double read_num(bool& is_float);
  int read_num(IntBase base);
  std::string read_string();
  char unescape_char();
  std::string read_identifier();
  std::string read_operator();

  SourceLocation _loc{1, 1};

  const std::set<char> space_set = { ' ', '\t', '\n' };
  const std::set<char> op_set = { '+', '-', '*', '/' , '<', '>', '=', ':' };
  const std::unordered_map<char, char> escapeMap = {
    { '\\', '\\' },
    { '\'', '\'' },
    { 'n',  '\n' },
    { 't',  '\t' },
    { 'r',  '\r' },
  };
};
