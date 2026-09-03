#include "lexer.h"
#include <cctype>

Lexer::Lexer(std::string source) : code(source), pos(0), line(1) {}

Token Lexer::getNextToken() {
    while (pos < code.length() && isspace(code[pos])) {
        if (code[pos] == '\n') line++; // Contamos las líneas
        pos++;
    }
    if (pos >= code.length()) return {TOKEN_EOF, "EOF"};

    char current = code[pos];

    if (current == '=') {
        if (pos + 1 < code.length() && code[pos + 1] == '=') { pos += 2; return {TOKEN_EQ, "=="}; }
        pos++; return {TOKEN_ASSIGN, "="};
    }
    if (current == ';') { pos++; return {TOKEN_SEMICOLON, ";"}; }
    if (current == '(') { pos++; return {TOKEN_LPAREN, "("}; }
    if (current == ')') { pos++; return {TOKEN_RPAREN, ")"}; }
    if (current == '{') { pos++; return {TOKEN_LBRACE, "{"}; }
    if (current == '}') { pos++; return {TOKEN_RBRACE, "}"}; }
    if (current == ',') { pos++; return {TOKEN_COMMA, ","}; }
    if (current == '<') { pos++; return {TOKEN_LT, "<"}; }
    if (current == '>') { pos++; return {TOKEN_GT, ">"}; }
    if (current == '!' && pos + 1 < code.length() && code[pos + 1] == '=') {
        pos += 2; return {TOKEN_NEQ, "!="};
    }

    if (current == '"') {
        std::string strValue = "\""; 
        pos++; 
        while (pos < code.length() && code[pos] != '"') {
            if (code[pos] == '\n') line++; 
            strValue += code[pos++];
        }
        if (pos < code.length()) {
            strValue += '"'; 
            pos++; 
        }
        return {TOKEN_STRING, strValue};
    }

    if (isdigit(current)) {
        std::string num = "";
        while (pos < code.length() && isdigit(code[pos])) num += code[pos++];
        return {TOKEN_NUMBER, num};
    }

    if (isalpha(current) || current == '_') {
        std::string word = "";
        while (pos < code.length() && (isalnum(code[pos]) || code[pos] == '_')) word += code[pos++];
        
        if (word == "var") return {TOKEN_VAR, word};
        if (word == "if") return {TOKEN_IF, word};
        if (word == "else") return {TOKEN_ELSE, word};
        if (word == "repeat") return {TOKEN_REPEAT, word};
        if (word == "exists") return {TOKEN_EXISTS, word};
        
        if (word == "move_file") return {TOKEN_CMD_MOVE, word};
        if (word == "copy_file") return {TOKEN_CMD_COPY, word};
        if (word == "create_file") return {TOKEN_CMD_CREATE, word};
        if (word == "delete_file") return {TOKEN_CMD_DELETE, word};
        if (word == "print") return {TOKEN_CMD_PRINT, word};
        if (word == "wait") return {TOKEN_CMD_WAIT, word};

        return {TOKEN_ID, word};
    }
    pos++;
    return {TOKEN_ERROR, std::string(1, current)};
}