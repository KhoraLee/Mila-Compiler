#pragma once

#include <set>
#include <string>

struct SourceLocation {
    int line;
    int column;
    SourceLocation(int l, int c) : line(l), column(c) {}
};

/*
 * Lexer returns tokens [0-255] if it is an unknown character, otherwise one of these for known things.
 * Here are all valid tokens:
 */
enum TokenType {
    // Special tokens
    TOK_INVALID = 0,
    TOK_EOF,
    
    // Keywords - Program structure
    TOK_PROGRAM,
    TOK_BEGIN,
    TOK_END,
    TOK_FORWARD,
    
    // Keywords - Declarations
    TOK_VAR,
    TOK_CONST,
    TOK_FUNCTION,
    TOK_PROCEDURE,
    TOK_ARRAY,
    TOK_OF,
    
    // Keywords - Control flow
    TOK_IF,
    TOK_THEN,
    TOK_ELSE,
    TOK_FOR,
    TOK_TO,
    TOK_DOWNTO,
    TOK_DO,
    TOK_WHILE,
    TOK_BREAK,
    TOK_EXIT,
    
    // Keywords - Data types
    TOK_INTEGER,
    TOK_FLOAT,
    TOK_STRING,
    TOK_VOID,
    
    // Operators - Arithmetic
    TOK_PLUS,
    TOK_MINUS,
    TOK_MULTIPLY,
    TOK_DIVIDE,
    TOK_DIV,
    TOK_MOD,
    
    // Operators - Comparison
    TOK_EQUAL,
    TOK_NOT_EQUAL,
    TOK_LESS,
    TOK_LESS_OR_EQUAL,
    TOK_GREATER,
    TOK_GREATER_OR_EQUAL,
    
    // Operators - Logical
    TOK_AND,
    TOK_OR,
    
    // Operators - Assignment
    TOK_ASSIGN,

    // Operator - Unary
    TOK_NOT,
    
    // Delimiters
    TOK_SEMICOLON,
    TOK_COMMA,
    TOK_COLON,
    TOK_DOT,
    TOK_OPEN_PAREN,
    TOK_CLOSE_PAREN,
    TOK_OPEN_BRACKET,
    TOK_CLOSE_BRACKET,
    
    // Identifiers and literals
    TOK_IDENTIFIER
};

enum IntBase {
  OCT = 8,
  HEX = 16,
};
