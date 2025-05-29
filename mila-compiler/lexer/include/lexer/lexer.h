#pragma once

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
