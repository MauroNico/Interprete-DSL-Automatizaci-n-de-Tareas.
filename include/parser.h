#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"
#include <memory>
#include <vector>
#include <string>

class Parser {
private:
    Lexer lexer;
    Token currentToken;

    void eat(TokenType expected);

    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<VarDeclNode> parseVariableDecl();
    std::unique_ptr<ASTNode> parseAssignment(); 
    std::unique_ptr<CommandCallNode> parseCommandCall();
    std::unique_ptr<ASTNode> parseRepeatStatement();
    std::unique_ptr<ASTNode> parseIfStatement();
    std::unique_ptr<ConditionNode> parseCondition(); 

public:
    Parser(std::string source);
    std::unique_ptr<ProgramNode> parseProgram();
};

#endif