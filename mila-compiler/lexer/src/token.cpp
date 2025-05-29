#include <lexer/token.h>
#include <lexer/token_map.h>

int OperatorToken::precedence() const {
  auto it = operators.find(_type);
  if (it == operators.end())
      return -1;
  return it->second.first;
}
