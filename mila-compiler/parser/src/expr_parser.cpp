#include <parser/parser.h>

Expr Parser::expression() {
    Expr expr = simpleExpression();
    
    if (match(TokenType::TOK_EQUAL) || match(TokenType::TOK_NOT_EQUAL) ||
        match(TokenType::TOK_LESS) || match(TokenType::TOK_LESS_OR_EQUAL) ||
        match(TokenType::TOK_GREATER) || match(TokenType::TOK_GREATER_OR_EQUAL)) {
        auto op = std::static_pointer_cast<OperatorToken>(_previous);
        Expr right = simpleExpression();
        return std::make_shared<BinaryExpr>(expr, op, right, expr->location());
    }
    
    return expr;
}

Expr Parser::simpleExpression() {
    Expr expr = term();
    
    while (match(TokenType::TOK_PLUS) || match(TokenType::TOK_MINUS) ||
           match(TokenType::TOK_OR)) {
        auto op = std::static_pointer_cast<OperatorToken>(_previous);
        Expr right = term();
        expr = std::make_shared<BinaryExpr>(expr, op, right, expr->location());
    }
    
    return expr;
}

Expr Parser::term() {
    Expr expr = factor();
    
    while (match(TokenType::TOK_MULTIPLY) || match(TokenType::TOK_DIVIDE) ||
           match(TokenType::TOK_DIV) || match(TokenType::TOK_MOD) ||
           match(TokenType::TOK_AND)) {
        auto op = std::static_pointer_cast<OperatorToken>(_previous);
        Expr right = factor();
        expr = std::make_shared<BinaryExpr>(expr, op, right, expr->location());
    }
    
    return expr;
}

Expr Parser::factor() {
    if (match(TokenType::TOK_NOT) || match(TokenType::TOK_MINUS) ||
        match(TokenType::TOK_PLUS)) {
        auto op = std::static_pointer_cast<OperatorToken>(_previous);
        Expr right = primary();
        return std::make_shared<UnaryExpr>(op, right, op->location());
    }
    
    return primary();
}

Expr Parser::primary() {
    if(match(TokenType::TOK_INTEGER)) {
        auto token = static_pointer_cast<IntToken>(_previous);
        return std::make_shared<IntegerLiteral>(token->value(), token->location());
    } else if(match(TokenType::TOK_FLOAT)) {
        auto token = static_pointer_cast<FloatToken>(_previous);
        return std::make_shared<FloatLiteral>(token->value(), token->location());
    } else if(match(TokenType::TOK_STRING)) {
        auto token = static_pointer_cast<StringToken>(_previous);
        return std::make_shared<StringLiteral>(token->string(), token->location());
    }
    
    if (match(TokenType::TOK_IDENTIFIER)) {
        return variableOrCall();
    }
    
    if (match(TokenType::TOK_OPEN_PAREN)) {
        Expr expr = expression();
        consume(TokenType::TOK_CLOSE_PAREN, "Expect ')' after expression.");
        return expr;
    }
    
    throw SyntaxErrorException(_current->location(), "Expected expression"); // but found '" + tokenToString(_current) + "'."
}

Expr Parser::variableOrCall() {
    auto token = std::static_pointer_cast<IdentifierToken>(_previous);
    std::string name = token->name();

    Expr expr = std::make_shared<VariableExpr>(name, token->location());

    if (match(TokenType::TOK_OPEN_PAREN)) {
        std::vector<Expr> args;
        if (!check(TokenType::TOK_CLOSE_PAREN)) {
            do {
                args.push_back(expression());
            } while (match(TokenType::TOK_COMMA));
        }
        consume(TokenType::TOK_CLOSE_PAREN, "Expect ')' after arguments.");
        expr = std::make_shared<CallExpr>(name, std::move(args), token->location());
        return expr;
    }

    if (match(TokenType::TOK_OPEN_BRACKET)) {
        Expr index = expression();
        consume(TokenType::TOK_CLOSE_BRACKET, "Expect ']' after array index.");
        expr = std::make_shared<ArrayAccess>(name, index, token->location());
    }

    return expr;
}
