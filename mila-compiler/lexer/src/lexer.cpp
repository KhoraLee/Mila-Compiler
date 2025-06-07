#include <exception/exception.h>
#include <lexer/lexer.h>
#include <lexer/token_map.h>

#include <iostream>
#include <sstream>

Lexer::Lexer(std::istream& input) : stream(input), current(stream.get()), prev_token(nullptr) { }

char Lexer::read_char() {
  current = stream.get();
  if (current == '\n') {
    _loc.line++;
    _loc.column = 1;
  } else {
    _loc.column++;
  }
  return current;
}

double Lexer::read_num(bool& is_float) {
  std::string number{current};
  bool dot_found = false;

  while (std::isdigit(read_char()) || current == '.') {
    if (current == '.') {
      if (dot_found) {
        throw InvalidSymbolException("invalid suffix on floating constant");
      }
      dot_found = true;
      is_float = true;
    } 
    number += current;
  }
  return std::stod(number);
}

int Lexer::read_num(IntBase base) {
  std::stringstream number;
  int ret;
  while(std::isdigit(read_char())) {
    number << current;
  }

  switch (base) {
  case OCT:
    number >> std::oct >> ret;
    break;
  case HEX:
    number >> std::hex >> ret;
    break;
  }

  return ret;
}

std::string Lexer::read_identifier() {
  std::string identifier{current};
  while (std::isalpha(read_char()) || std::isdigit(current) || current == '_') {
    identifier += current;
  }
  return identifier;
}

std::string Lexer::read_operator() {
  if (current == '=') { 
    read_char();
    return "=";
  }
  std::string op{current};
  if (op_set.contains(read_char())) {
    op += current;
    read_char();
  }
  return op;
}

std::string Lexer::read_string() {
  throw Exception("Not yet implemented");
}

template<typename T, typename U>
TokenPtr make_token(const U arg, const SourceLocation loc) {
  return std::static_pointer_cast<Token>(std::make_shared<T>(arg, loc));
}

TokenPtr Lexer::next_token() {
  while(space_set.contains(current)) { // Remove ' ', '\t', '\n'
    read_char();
  }

  SourceLocation tok_start = _loc;

  switch(current) {
  case EOF:
    return make_token<BasicToken>(TOK_EOF, tok_start);
  case 'a' ... 'z':
  case 'A' ... 'Z': {
    const std::string identifier = read_identifier();
    auto it = keyword_map.find(identifier);
    if (it != keyword_map.end()) {
      return make_token<BasicToken>(it->second, tok_start);
    }
    it = operator_map.find(identifier);
    if (it != operator_map.end()) {
      return make_token<OperatorToken>(it->second, tok_start);
    }
    return make_token<IdentifierToken>(identifier, tok_start);
  }
  // number
  case '0' ... '9': {
    bool is_float = false;
    double num = read_num(is_float);
    if (is_float) {
      return make_token<FloatToken>(num, tok_start);
    } else {
      return make_token<IntToken>(num, tok_start);
    }
  }
  case '&': // oct
    return make_token<IntToken>(read_num(IntBase::OCT), tok_start);
  case '$': // hex
    return make_token<IntToken>(read_num(IntBase::HEX), tok_start);
  case '\'': // string
    return make_token<StringToken>(read_string(), tok_start);
  case '<':
  case '=':
  case '>':
  case '+':
  case '-':
  case '*':
  case '/':
  case ':': {
    const std::string op = read_operator();
    auto it = operator_map.find(op);
    if (it != operator_map.end()) {
      return make_token<OperatorToken>(it->second, tok_start);
    }
    auto it2 = symbol_map.find(op[0]);
    if (it2 != symbol_map.end() && op.length() == 1) {
      return make_token<BasicToken>(it2->second, tok_start);
    }
    throw InvalidSymbolException("Invalid symbol");
  }  
  default: {
    auto it = symbol_map.find(current);
    if (it != symbol_map.end()) {
      read_char();
      return make_token<OperatorToken>(it->second, tok_start);
    } else {
      throw InvalidSymbolException("Invalid symbol");
    }
  }
  }
}
