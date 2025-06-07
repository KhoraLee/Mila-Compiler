#include <parser/parser.h>

Stmt Parser::statement() {
    if (match(TokenType::TOK_IF)) return ifStatement();
    if (match(TokenType::TOK_WHILE)) return whileStatement();
    if (match(TokenType::TOK_FOR)) return forStatement();
    if (match(TokenType::TOK_BEGIN)) return blockStatement();
    if (match(TokenType::TOK_BREAK)) return breakStatement();
    if (match(TokenType::TOK_EXIT)) return exitStatement();
    
    // Check for assignment or call statement
    if (check(TokenType::TOK_IDENTIFIER)) {
        auto token = std::static_pointer_cast<IdentifierToken>(_current);
        advance();
        
        // Check if it's a function call
        if (match(TokenType::TOK_OPEN_PAREN)) {
            return callStatement(token->name());
        }
        
        // Otherwise it's an assignment
        return assignment(token->name());
    }
    
    throw SyntaxErrorException(_current->location(), "Expected statement");
}

Assign_S Parser::assignment(const std::string& target) {
    consume(TokenType::TOK_ASSIGN, "Expected ':=' after variable name");
    Expr value = expression();
    consume(TokenType::TOK_SEMICOLON, "Expected ';' after assignment");
    return std::make_shared<AssignStmt>(target, value, _previous->location());
}

If_S Parser::ifStatement() {
    Expr condition = expression();
    consume(TokenType::TOK_THEN, "Expected 'then' after if condition");
    Stmt thenBranch = statement();
    
    Stmt elseBranch = nullptr;
    if (match(TokenType::TOK_ELSE)) {
        elseBranch = statement();
    }
    
    return std::make_shared<IfStmt>(condition, thenBranch, elseBranch, _previous->location());
}

While_S Parser::whileStatement() {
    Expr condition = expression();
    consume(TokenType::TOK_DO, "Expected 'do' after while condition");
    Stmt body = statement();
    return std::make_shared<WhileStmt>(condition, body, _previous->location());
}

For_S Parser::forStatement() {
    consume(TokenType::TOK_IDENTIFIER, "Expected loop variable name");
    auto loopVar = std::static_pointer_cast<IdentifierToken>(_previous)->name();
    
    consume(TokenType::TOK_ASSIGN, "Expected ':=' after loop variable");
    Expr start = expression();
    
    bool downto = false;
    if (match(TokenType::TOK_TO)) {
        downto = false;
    } else if (match(TokenType::TOK_DOWNTO)) {
        downto = true;
    } else {
        throw SyntaxErrorException(_current->location(), "Expected 'to' or 'downto'");
    }
    
    Expr end = expression();
    consume(TokenType::TOK_DO, "Expected 'do' after for range");
    Stmt body = statement();
    
    return std::make_shared<ForStmt>(loopVar, start, end, body, downto, _previous->location());
}

Block_S Parser::blockStatement() {
    std::vector<Stmt> statements;
    
    while (!check(TokenType::TOK_END) && !isAtEnd()) {
        statements.push_back(statement());
    }
    
    consume(TokenType::TOK_END, "Expected 'end' after block");
    return std::make_shared<BlockStmt>(statements, _previous->location());
}

Call_S Parser::callStatement(const std::string& callee) {
    std::vector<Expr> args;
    
    if (!check(TokenType::TOK_CLOSE_PAREN)) {
        do {
            args.push_back(expression());
        } while (match(TokenType::TOK_COMMA));
    }
    
    consume(TokenType::TOK_CLOSE_PAREN, "Expected ')' after arguments");
    consume(TokenType::TOK_SEMICOLON, "Expected ';' after call statement");
    
    return std::make_shared<CallStmt>(callee, args, _previous->location());
}

Break_S Parser::breakStatement() {
    consume(TokenType::TOK_SEMICOLON, "Expected ';' after break");
    return std::make_shared<BreakStmt>(_previous->location());
}

Exit_S Parser::exitStatement() {
    consume(TokenType::TOK_SEMICOLON, "Expected ';' after exit");
    return std::make_shared<ExitStmt>(_previous->location());
}
