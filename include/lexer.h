#ifndef LEXER_H
#define LEXER_H

#include <string>

enum TokenType {
    TOKEN_VAR, TOKEN_IF, TOKEN_ELSE, TOKEN_REPEAT, TOKEN_EXISTS,
    TOKEN_CMD_CREATE, TOKEN_CMD_DELETE, TOKEN_CMD_COPY, 
    TOKEN_CMD_MOVE, TOKEN_CMD_PRINT, TOKEN_CMD_WAIT,
    TOKEN_ID, TOKEN_NUMBER, TOKEN_STRING,
    TOKEN_EQ, TOKEN_NEQ, TOKEN_LT, TOKEN_GT,
    TOKEN_ASSIGN, TOKEN_LPAREN, TOKEN_RPAREN, 
    TOKEN_LBRACE, TOKEN_RBRACE, TOKEN_COMMA, TOKEN_SEMICOLON,
    TOKEN_EOF, TOKEN_ERROR
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
};

class Lexer {
private:
    std::string code;
    size_t pos;
    int line;

public:
    Lexer(const std::string& source);
    Token getNextToken();
    int getLine() const { return line; } 
};

#endif