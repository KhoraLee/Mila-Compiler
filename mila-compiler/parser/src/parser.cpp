#include <parser/parser.h>

Parser::Parser(Lexer& lexer) : _lexer(lexer) {
    advance(); // Initialize with first token
}

Program_D Parser::parse() {
    try {
        return programDeclaration();
    } catch (const MilaException& e) {
        std::cerr << e.what() << std::endl;
        // TODO: Implement error hanlding
        return nullptr;
    }
}

void Parser::advance() {
    _previous = _current;
    _current = _lexer.next_token();
}

bool Parser::check(TokenType type) const {
    return !isAtEnd() && _current->type() == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

void Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        return advance();
    }
    if (isAtEnd()) {
        throw MissingTokenException(_current->location(), message);
    } else {
        throw UnexpectedTokenException(_current->location(), message);
    }
}

bool Parser::isAtEnd() const {
    return _current->type() == TokenType::TOK_EOF;
}
