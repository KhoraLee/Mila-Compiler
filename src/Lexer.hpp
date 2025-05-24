#ifndef PJPPROJECT_LEXER_HPP
#define PJPPROJECT_LEXER_HPP

#include <iostream>
#include <string>
#include <cctype>
#include <map>

class Lexer {
public:
    Lexer(std::istream& input = std::cin);
    ~Lexer() = default;

    int getToken();
    const std::string& identifierStr() const { return m_IdentifierStr; }
    int64_t numVal() const { return m_NumVal; }
    const std::string& stringVal() const { return m_StringVal; }

private:
    // Helper methods
    void advance() { m_LastChar = m_Input.get(); }
    char peek() { 
        char next = m_Input.peek();
        return next == EOF ? '\0' : next;
    }
    void skipWhitespace();
    int readNumber();
    std::string readIdentifier();
    int readString();
    void skipComment();

    // Member variables
    std::istream& m_Input;
    std::string m_IdentifierStr;
    int64_t m_NumVal;
    std::string m_StringVal;
    int m_LastChar = ' ';
    std::map<std::string, int> m_Keywords;
};

/*
 * Lexer returns tokens [0-255] if it is an unknown character, otherwise one of these for known things.
 * Here are all valid tokens:
 */
enum Token {
    // Keywords
    tok_program = -1,
    tok_var = -2,
    tok_array = -3,
    tok_of = -4,
    tok_begin = -5,
    tok_end = -6,
    tok_for = -7,
    tok_do = -8,
    tok_if = -9,
    tok_then = -10,
    tok_else = -11,
    tok_to = -12,
    tok_downto = -13,
    tok_div = -14,
    tok_mod = -15,
    tok_const = -16,
    tok_function = -17,
    tok_while = -18,
    tok_procedure = -19,
    tok_exit = -20,
    tok_or = -21,
    tok_and = -22,
    tok_not = -23,
    tok_forward = -24,
    tok_break = -25,

    // Single character tokens
    tok_colon = -26,      // :
    tok_semicolon = -27,  // ;
    tok_assign = -28,     // :=
    tok_sqopen = -29,     // [
    tok_sqclose = -30,    // ]
    tok_paropen = -31,    // (
    tok_parclose = -32,   // )
    tok_dotdot = -33,     // ..
    tok_dot = -34,        // .
    tok_lt = -35,         // <
    tok_gt = -36,         // >
    tok_eq = -37,         // =
    tok_ne = -38,         // <>
    tok_le = -39,         // <=
    tok_ge = -40,         // >=
    tok_mul = -41,        // *
    tok_add = -42,        // +
    tok_sub = -43,        // -
    tok_comma = -44,      // ,

    // Special tokens
    tok_identifier = -45,
    tok_number = -46,
    tok_string = -47,
    tok_eof = -48
};

#endif //PJPPROJECT_LEXER_HPP
