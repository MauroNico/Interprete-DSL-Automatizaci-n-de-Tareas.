#include "parser.h"
#include <stdexcept>

Parser::Parser(std::string source) : lexer(source) {
    currentToken = lexer.getNextToken(); 
}

void Parser::eat(TokenType expected) {
    if (currentToken.type == expected) {
        currentToken = lexer.getNextToken();
    } else {
        throw std::runtime_error("Error sintactico en linea " + std::to_string(lexer.getLine()) + 
                                 ": Token inesperado. Lexema: " + currentToken.lexeme);
    }
}

std::unique_ptr<ProgramNode> Parser::parseProgram() {
    std::vector<std::unique_ptr<ASTNode>> statements;
    while (currentToken.type != TOKEN_EOF) statements.push_back(parseStatement());
    return std::make_unique<ProgramNode>(std::move(statements));
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    if (currentToken.type == TOKEN_VAR) return parseVariableDecl();
    else if (currentToken.type == TOKEN_ID) return parseAssignment(); 
    else if (currentToken.type == TOKEN_CMD_CREATE || currentToken.type == TOKEN_CMD_DELETE || 
             currentToken.type == TOKEN_CMD_MOVE || currentToken.type == TOKEN_CMD_COPY || 
             currentToken.type == TOKEN_CMD_WAIT || currentToken.type == TOKEN_CMD_PRINT) {
        return parseCommandCall();
    } 
    else if (currentToken.type == TOKEN_REPEAT) return parseRepeatStatement();
    else if (currentToken.type == TOKEN_IF) return parseIfStatement();
    else throw std::runtime_error("Error sintactico en linea " + std::to_string(lexer.getLine()) + ": Sentencia no reconocida.");
}

std::unique_ptr<VarDeclNode> Parser::parseVariableDecl() {
    eat(TOKEN_VAR);
    std::string id = currentToken.lexeme;
    eat(TOKEN_ID);
    eat(TOKEN_ASSIGN);
    
    std::string expr = currentToken.lexeme; 
    if(currentToken.type == TOKEN_STRING) eat(TOKEN_STRING);
    else if(currentToken.type == TOKEN_NUMBER) eat(TOKEN_NUMBER);
    else eat(TOKEN_ID); 
    
    eat(TOKEN_SEMICOLON);
    return std::make_unique<VarDeclNode>(id, expr);
}

// asignar a variables ya creadas
std::unique_ptr<ASTNode> Parser::parseAssignment() {
    std::string id = currentToken.lexeme;
    eat(TOKEN_ID);
    eat(TOKEN_ASSIGN);
    
    std::string expr = currentToken.lexeme; 
    if(currentToken.type == TOKEN_STRING) eat(TOKEN_STRING);
    else if(currentToken.type == TOKEN_NUMBER) eat(TOKEN_NUMBER);
    else eat(TOKEN_ID); 
    
    eat(TOKEN_SEMICOLON);
    return std::make_unique<AssignmentNode>(id, expr);
}

std::unique_ptr<CommandCallNode> Parser::parseCommandCall() {
    std::string cmd = currentToken.lexeme;
    eat(currentToken.type); 
    eat(TOKEN_LPAREN);
    
    std::vector<std::string> args;
    
    if (currentToken.type != TOKEN_RPAREN) {
        args.push_back(currentToken.lexeme);
        if(currentToken.type == TOKEN_STRING) eat(TOKEN_STRING);
        else if (currentToken.type == TOKEN_NUMBER) eat(TOKEN_NUMBER); // Añadido para permitir números (ej. wait(500))
        else eat(TOKEN_ID);
        
        while (currentToken.type == TOKEN_COMMA) {
            eat(TOKEN_COMMA);
            args.push_back(currentToken.lexeme);
            if(currentToken.type == TOKEN_STRING) eat(TOKEN_STRING);
            else if (currentToken.type == TOKEN_NUMBER) eat(TOKEN_NUMBER);
            else eat(TOKEN_ID);
        }
    }
    
    eat(TOKEN_RPAREN);
    eat(TOKEN_SEMICOLON);
    
    return std::make_unique<CommandCallNode>(cmd, args);
}

std::unique_ptr<ASTNode> Parser::parseRepeatStatement() {
    eat(TOKEN_REPEAT);
    eat(TOKEN_LPAREN);
    
    std::string iters = currentToken.lexeme;
    // ahora puede ser var o numeros
    if (currentToken.type == TOKEN_NUMBER) eat(TOKEN_NUMBER);
    else eat(TOKEN_ID); 
    
    eat(TOKEN_RPAREN);
    eat(TOKEN_LBRACE);
    
    std::vector<std::unique_ptr<ASTNode>> innerStmts;
    while (currentToken.type != TOKEN_RBRACE && currentToken.type != TOKEN_EOF) {
        innerStmts.push_back(parseStatement());
    }
    eat(TOKEN_RBRACE);
    
    auto bodyNode = std::make_unique<ProgramNode>(std::move(innerStmts));
    return std::make_unique<RepeatNode>(iters, std::move(bodyNode));
}

std::unique_ptr<ASTNode> Parser::parseIfStatement() {
    eat(TOKEN_IF);
    eat(TOKEN_LPAREN);
    
    auto condNode = parseCondition();
    
    eat(TOKEN_RPAREN);
    eat(TOKEN_LBRACE);
    
    std::vector<std::unique_ptr<ASTNode>> thenStmts;
    while (currentToken.type != TOKEN_RBRACE && currentToken.type != TOKEN_EOF) {
        thenStmts.push_back(parseStatement());
    }
    eat(TOKEN_RBRACE);
    
    std::vector<std::unique_ptr<ASTNode>> elseStmts;
    if (currentToken.type == TOKEN_ELSE) {
        eat(TOKEN_ELSE);
        eat(TOKEN_LBRACE);
        while (currentToken.type != TOKEN_RBRACE && currentToken.type != TOKEN_EOF) {
            elseStmts.push_back(parseStatement());
        }
        eat(TOKEN_RBRACE);
    }
    
    auto thenNode = std::make_unique<ProgramNode>(std::move(thenStmts));
    auto elseNode = elseStmts.empty() ? nullptr : std::make_unique<ProgramNode>(std::move(elseStmts));
    
    return std::make_unique<IfNode>(std::move(condNode), std::move(thenNode), std::move(elseNode));
}

std::unique_ptr<ConditionNode> Parser::parseCondition() {
    if (currentToken.type == TOKEN_EXISTS) {
        eat(TOKEN_EXISTS);
        eat(TOKEN_LPAREN);
        std::string expr = currentToken.lexeme;
        if(currentToken.type == TOKEN_STRING) eat(TOKEN_STRING);
        else eat(TOKEN_ID);
        eat(TOKEN_RPAREN);
        return std::make_unique<ConditionNode>("exists", expr, "");
    } else {
        std::string leftExpr = currentToken.lexeme;
        if(currentToken.type == TOKEN_STRING) eat(TOKEN_STRING);
        else if(currentToken.type == TOKEN_NUMBER) eat(TOKEN_NUMBER);
        else eat(TOKEN_ID);
        
        std::string op = currentToken.lexeme;
        if (currentToken.type == TOKEN_EQ) eat(TOKEN_EQ);
        else if (currentToken.type == TOKEN_NEQ) eat(TOKEN_NEQ);
        else if (currentToken.type == TOKEN_LT) eat(TOKEN_LT);
        else if (currentToken.type == TOKEN_GT) eat(TOKEN_GT);
        else throw std::runtime_error("Error sintactico en linea " + std::to_string(lexer.getLine()) + ": Se esperaba un operador relacional");
        
        std::string rightExpr = currentToken.lexeme;
        if(currentToken.type == TOKEN_STRING) eat(TOKEN_STRING);
        else if(currentToken.type == TOKEN_NUMBER) eat(TOKEN_NUMBER);
        else eat(TOKEN_ID);
        
        return std::make_unique<ConditionNode>(op, leftExpr, rightExpr);
    }
}