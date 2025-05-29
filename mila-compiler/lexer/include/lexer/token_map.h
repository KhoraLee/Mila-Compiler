#pragma once

#include <lexer/types.h>

#include <set>

const std::unordered_map<char, TokenType> symbol_map = {
    { '=', TOK_EQUAL },
    { '.', TOK_DOT },
    { ';', TOK_SEMICOLON },
    { '(', TOK_OPEN_BRACKET },
    { ')', TOK_CLOSE_BRACKET },
    { ',', TOK_COMMA },
    { ':', TOK_COLON }
};

const std::unordered_map<std::string, TokenType> keyword_map = {
  // Keywords - Program structure
  { "program", TOK_PROGRAM },
  { "begin", TOK_BEGIN },
  { "end", TOK_END },
  { "forward", TOK_FORWARD },

  // Keywords - Declarations
  { "var", TOK_VAR },
  { "const", TOK_CONST },
  { "function", TOK_FUNCTION },
  { "procedure", TOK_PROCEDURE },

  // Keywords - Control flow
  { "if", TOK_IF },
  { "then", TOK_THEN },
  { "else", TOK_ELSE },
  { "for", TOK_FOR },
  { "to", TOK_TO },
  { "downto", TOK_DOWNTO },
  { "do", TOK_DO },
  { "while", TOK_WHILE },
  { "break", TOK_BREAK },
  { "exit", TOK_EXIT },

  // Keywords - Data types
  { "integer", TOK_INTEGER },
  { "float", TOK_FLOAT },
  { "string", TOK_STRING },
};

const std::unordered_map<std::string, TokenType> operator_map = {
  // Assignment
  { ":=", TOK_ASSIGN },

  // Arithmetic
  { "+", TOK_PLUS },
  { "-", TOK_MINUS },
  { "*", TOK_MULTIPLY },
  { "/", TOK_DIVIDE },

  { "div", TOK_DIV },
  { "mod", TOK_MOD },

  // Comparison
  { "=", TOK_EQUAL },
  { "<", TOK_LESS },
  { "<=", TOK_LESS_OR_EQUAL },
  { ">", TOK_GREATER },
  { ">=", TOK_GREATER_OR_EQUAL },
  { "<>", TOK_NOT_EQUAL },

  // Logical
  { "and", TOK_AND },
  { "or", TOK_OR },
};

const std::unordered_map<TokenType, std::pair<int, bool>> operators = {
  // Assignment
  { TOK_ASSIGN, { 5, false } },
  
  // Arithmetic
  { TOK_PLUS, { 20, false } },
  { TOK_MINUS, { 20, false } },
  { TOK_MULTIPLY, { 40, false } },
  { TOK_DIVIDE, {40, false } },
  
  { TOK_DIV, { 40, false } },
  { TOK_MOD, { 40, false } },

  // Comparison
  { TOK_EQUAL, { 10, true } },
  { TOK_LESS, { 10, true } },
  { TOK_LESS_OR_EQUAL, { 10, true } },
  { TOK_GREATER, { 10, true } },
  { TOK_GREATER_OR_EQUAL, { 10, true } },
  { TOK_NOT_EQUAL, { 10, true } },

  // Logical
  { TOK_AND, { 10, true } },
  { TOK_OR, { 10, true } },
};
