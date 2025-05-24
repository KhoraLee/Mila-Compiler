#include "Lexer.hpp"
#include <algorithm>

Lexer::Lexer(std::istream& input) : m_Input(input) {
    // Initialize keywords
    m_Keywords["program"] = tok_program;
    m_Keywords["var"] = tok_var;
    m_Keywords["array"] = tok_array;
    m_Keywords["of"] = tok_of;
    m_Keywords["begin"] = tok_begin;
    m_Keywords["end"] = tok_end;
    m_Keywords["for"] = tok_for;
    m_Keywords["do"] = tok_do;
    m_Keywords["if"] = tok_if;
    m_Keywords["then"] = tok_then;
    m_Keywords["else"] = tok_else;
    m_Keywords["to"] = tok_to;
    m_Keywords["downto"] = tok_downto;
    m_Keywords["div"] = tok_div;
    m_Keywords["mod"] = tok_mod;
    m_Keywords["const"] = tok_const;
    m_Keywords["function"] = tok_function;
    m_Keywords["while"] = tok_while;
    m_Keywords["procedure"] = tok_procedure;
    m_Keywords["exit"] = tok_exit;
    m_Keywords["or"] = tok_or;
    m_Keywords["and"] = tok_and;
    m_Keywords["not"] = tok_not;
    m_Keywords["forward"] = tok_forward;
    m_Keywords["break"] = tok_break;
}

void Lexer::skipWhitespace() {
    while (isspace(m_LastChar)) {
        advance();
    }
}

int Lexer::readNumber() {
    std::string NumStr;
    int base = 10;

    if (m_LastChar == '$') {
        base = 16;
        advance();
    } else if (m_LastChar == '&') {
        base = 8;
        advance();
    }

    // Read digits based on the number base
    while (m_LastChar != '\0' && 
           ((base == 10 && isdigit(m_LastChar)) ||
            (base == 16 && isxdigit(m_LastChar)) ||
            (base == 8 && m_LastChar >= '0' && m_LastChar <= '7'))) {
        NumStr += m_LastChar;
        advance();
    }

    // If we didn't read any valid digits, return error
    if (NumStr.empty()) {
        return tok_eof;
    }

    try {
        m_NumVal = std::stoll(NumStr, nullptr, base);
    } catch (const std::exception&) {
        return tok_eof;
    }

    return tok_number;
}

std::string Lexer::readIdentifier() {
    std::string identifier;
    
    while (m_LastChar != '\0' && 
            (isalnum(m_LastChar) || m_LastChar == '_')) {
        identifier += tolower(m_LastChar);  // Convert to lowercase for case-insensitive matching
        advance();
    }
    
    return identifier;
}

int Lexer::readString() {
    m_StringVal.clear();
    advance();
    
    while (m_LastChar != '\'' && m_LastChar != EOF) {
        m_StringVal += m_LastChar;
        advance();
    }
    
    if (m_LastChar == '\'') {
        advance();
        return tok_string;
    }
    return tok_eof;
}

void Lexer::skipComment() {
    do {
        advance();
    } while (m_LastChar != '}' && m_LastChar != EOF);
    
    if (m_LastChar == '}') {
        advance();
    }
}

int Lexer::getToken() {
    skipWhitespace();

    // Identifier: [a-zA-Z][a-zA-Z0-9_]*
    if (isalpha(m_LastChar)) {
        m_IdentifierStr = readIdentifier();
        
        // Check if the identifier is a keyword
        auto it = m_Keywords.find(m_IdentifierStr);
        if (it != m_Keywords.end()) {
            return it->second;
        }
        return tok_identifier;
    }

    // Number: [0-9]+ or $[0-9A-Fa-f]+ or &[0-7]+
    if (isdigit(m_LastChar) || m_LastChar == '$' || m_LastChar == '&') {
        return readNumber();
    }

    // String literal: '...'
    if (m_LastChar == '\'') {
        return readString();
    }

    // Handle comments: { ... }
    if (m_LastChar == '{') {
        skipComment();
        return getToken(); // Skip comment and get next token
    }

    // Handle two-character operators
    if (m_LastChar == ':') {
        advance();
        if (m_LastChar == '=') {
            advance();
            return tok_assign;
        }
        return tok_colon;
    }

    if (m_LastChar == '<') {
        advance();
        if (m_LastChar == '>') {
            advance();
            return tok_ne;
        }
        if (m_LastChar == '=') {
            advance();
            return tok_le;
        }
        return tok_lt;
    }

    if (m_LastChar == '>') {
        advance();
        if (m_LastChar == '=') {
            advance();
            return tok_ge;
        }
        return tok_gt;
    }

    if (m_LastChar == '.') {
        advance();
        if (m_LastChar == '.') {
            advance();
            return tok_dotdot;
        }
        return tok_dot;
    }

    // Handle single-character operators
    int ThisChar = m_LastChar;
    advance();
    
    switch (ThisChar) {
        case ';': return tok_semicolon;
        case '[': return tok_sqopen;
        case ']': return tok_sqclose;
        case '(': return tok_paropen;
        case ')': return tok_parclose;
        case '=': return tok_eq;
        case '*': return tok_mul;
        case '+': return tok_add;
        case '-': return tok_sub;
        case ',': return tok_comma;
        case EOF: return tok_eof;
    }

    return ThisChar;
}

