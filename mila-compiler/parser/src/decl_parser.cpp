#include <parser/parser.h>

Const_D Parser::constDeclaration() {
    consume(TokenType::TOK_IDENTIFIER, "Expected constant name");
    auto name = std::static_pointer_cast<IdentifierToken>(_previous)->name();
    
    consume(TokenType::TOK_EQUAL, "Expected '=' after constant name");
    Expr value = expression();
    consume(TokenType::TOK_SEMICOLON, "Expected ';' after constant value");
    
    return std::make_shared<ConstDecl>(name, value, _previous->location());
}

std::vector<Named_D> Parser::varDeclarations() {
    std::vector<Named_D> declarations;
    
    do {
        consume(TokenType::TOK_IDENTIFIER, "Expected variable name");
        auto name = std::static_pointer_cast<IdentifierToken>(_previous)->name();
        declarations.push_back(std::make_shared<VarDecl>(name, TokenType::TOK_VOID, _previous->location()));
    } while (match(TokenType::TOK_COMMA));
    
    consume(TokenType::TOK_COLON, "Expected ':' after variable list");
    
    // Check if it's an array declaration
    if (match(TokenType::TOK_ARRAY)) {
        consume(TokenType::TOK_OPEN_BRACKET, "Expected '[' after array");
        
        // Parse start index (can be negative)
        bool isNegative = match(TokenType::TOK_MINUS);
        consume(TokenType::TOK_INTEGER, "Expected integer literal for array start index");
        int start = std::static_pointer_cast<IntToken>(_previous)->value();
        if (isNegative) start = -start;
        
        consume(TokenType::TOK_DOT, "Expected '..' after array start index");
        consume(TokenType::TOK_DOT, "Expected second '.' in '..'");
        
        // Parse end index (can be negative)
        isNegative = match(TokenType::TOK_MINUS);
        consume(TokenType::TOK_INTEGER, "Expected integer literal for array end index");
        int end = std::static_pointer_cast<IntToken>(_previous)->value();
        if (isNegative) end = -end;
        
        consume(TokenType::TOK_CLOSE_BRACKET, "Expected ']' after array end index");
        consume(TokenType::TOK_OF, "Expected 'of' after array bounds");
        
        // Parse array element type
        TokenType type;
        if (match(TokenType::TOK_INTEGER) || match(TokenType::TOK_FLOAT) || match(TokenType::TOK_STRING)) {
            type = _previous->type();
        } else {
            throw SyntaxErrorException(_current->location(), "Expected array element type after 'of'");
        }
        
        // Update all declarations to be array declarations
        for (auto& decl : declarations) {
            decl = std::make_shared<ArrayDecl>(decl->name(), type, 
                start, end, false, decl->location());
        }
    } else {
        // Parse regular variable type
        TokenType type;
        if (match(TokenType::TOK_INTEGER) || match(TokenType::TOK_FLOAT) || match(TokenType::TOK_STRING)) {
            type = _previous->type();
        } else {
            throw SyntaxErrorException(_current->location(), "Expected type after ':'");
        }
        
        // Update all declarations with the correct type
        for (auto& decl : declarations) {
            decl = std::make_shared<VarDecl>(decl->name(), type, decl->location());
        }
    }
    
    consume(TokenType::TOK_SEMICOLON, "Expected ';' after variable declaration");
    
    // Return the first declaration (they're all the same type)
    return declarations;
}

Function_D Parser::functionDeclaration() {
    bool isProcedure = _previous->type() == TokenType::TOK_PROCEDURE;
    
    consume(TokenType::TOK_IDENTIFIER, "Expected function name");
    auto name = std::static_pointer_cast<IdentifierToken>(_previous)->name();
    
    // Parse parameters
    ParamList params;
    consume(TokenType::TOK_OPEN_PAREN, "Expected '(' after function name");
    if (!check(TokenType::TOK_CLOSE_PAREN)) {
        do {
            consume(TokenType::TOK_IDENTIFIER, "Expected parameter name");
            auto paramName = std::static_pointer_cast<IdentifierToken>(_previous)->name();
            
            consume(TokenType::TOK_COLON, "Expected ':' after parameter name");
            
            TokenType paramType;
            if (match(TokenType::TOK_INTEGER) || match(TokenType::TOK_FLOAT) || match(TokenType::TOK_STRING)) {
                paramType = _previous->type();
            } else {
                throw SyntaxErrorException(_current->location(), "Expected parameter type");
            }
            
            params.push_back({paramName, paramType});
        } while (match(TokenType::TOK_SEMICOLON));
    }
    consume(TokenType::TOK_CLOSE_PAREN, "Expected ')' after parameters");
    
    // Parse return type for functions
    TokenType returnType = TokenType::TOK_VOID;
    if (!isProcedure) {
        consume(TokenType::TOK_COLON, "Expected ':' after function parameters");
        if (match(TokenType::TOK_INTEGER) || match(TokenType::TOK_FLOAT) || match(TokenType::TOK_STRING)) {
            returnType = _previous->type();
        } else {
            throw SyntaxErrorException(_current->location(), "Expected parameter type");
        }
    }
    
    consume(TokenType::TOK_SEMICOLON, "Expected ';' after function declaration");
    
    // Parse const and var declarations
    std::vector<Named_D> consts;
    std::vector<Named_D> vars;

    // Parse const declarations
    if (match(TokenType::TOK_CONST)) {
        do {
            match(TokenType::TOK_CONST);
            consts.push_back(constDeclaration());
        } while (check(TokenType::TOK_CONST) || check(TokenType::TOK_IDENTIFIER));
    }

    // Parse var declarations
    if (match(TokenType::TOK_VAR)) {
        do {
            match(TokenType::TOK_VAR);
            auto newVars = varDeclarations();
            vars.insert(vars.end(), newVars.begin(), newVars.end());
        } while (check(TokenType::TOK_VAR) || check(TokenType::TOK_IDENTIFIER));
    }

    // Parse function body
    consume(TokenType::TOK_BEGIN, "Expected 'begin' for function body");
    Block_S body = blockStatement();
    
    return std::make_shared<FunctionDecl>(name, params, returnType, consts, vars, body, _previous->location());
}

Program_D Parser::programDeclaration() {
    consume(TokenType::TOK_PROGRAM, "Expected 'program'");
    consume(TokenType::TOK_IDENTIFIER, "Expected program name");
    auto name = std::static_pointer_cast<IdentifierToken>(_previous)->name();
    consume(TokenType::TOK_SEMICOLON, "Expected ';' after program name");

    // Parse function declarations
    std::vector<Function_D> functions;
    while (match(TokenType::TOK_FUNCTION) || match(TokenType::TOK_PROCEDURE)) {
        functions.push_back(functionDeclaration());
    }

    // Parse const and var declarations
    std::vector<Named_D> consts;
    std::vector<Named_D> vars;

    // Parse const declarations
    if (match(TokenType::TOK_CONST)) {
        do {
            match(TokenType::TOK_CONST);
            consts.push_back(constDeclaration());
        } while (check(TokenType::TOK_CONST) || check(TokenType::TOK_IDENTIFIER));
    }

    // Parse var declarations
    if (match(TokenType::TOK_VAR)) {
        do {
            match(TokenType::TOK_VAR);
            auto newVars = varDeclarations();
            vars.insert(vars.end(), newVars.begin(), newVars.end());
        } while (check(TokenType::TOK_VAR) || check(TokenType::TOK_IDENTIFIER));
    }

    // Parse program body
    consume(TokenType::TOK_BEGIN, "Expected 'begin' for program body");
    Block_S body = blockStatement(true);
    consume(TokenType::TOK_DOT, "Expected '.' after program end");

    
    return std::make_shared<ProgramDecl>(name, consts, vars, functions, body, _previous->location());
}
