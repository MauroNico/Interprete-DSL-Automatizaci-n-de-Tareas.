#include "lexer.h"
#include <cctype>
#include <unordered_map>

Lexer::Lexer(const std::string& source) : code(source), pos(0), line(1) {}

Token Lexer::getNextToken() {
    while (pos < code.length() && isspace(code[pos])) {
        if (code[pos] == '\n') line++;
        pos++;
    }
    if (pos >= code.length()) return {TOKEN_EOF, "EOF", line};

    char current = code[pos];

    // Comentarios de una línea
    if (current == '/' && pos + 1 < code.length() && code[pos + 1] == '/') {
        while (pos < code.length() && code[pos] != '\n') pos++;
        return getNextToken();
    }

    if (current == '=') {
        if (pos + 1 < code.length() && code[pos + 1] == '=') { pos += 2; return {TOKEN_EQ, "==", line}; }
        pos++; return {TOKEN_ASSIGN, "=", line};
    }
    if (current == '!' && pos + 1 < code.length() && code[pos + 1] == '=') {
        pos += 2; return {TOKEN_NEQ, "!=", line};
    }
    if (current == ';') { pos++; return {TOKEN_SEMICOLON, ";", line}; }
    if (current == '(') { pos++; return {TOKEN_LPAREN, "(", line}; }
    if (current == ')') { pos++; return {TOKEN_RPAREN, ")", line}; }
    if (current == '{') { pos++; return {TOKEN_LBRACE, "{", line}; }
    if (current == '}') { pos++; return {TOKEN_RBRACE, "}", line}; }
    if (current == ',') { pos++; return {TOKEN_COMMA, ",", line}; }
    if (current == '<') { pos++; return {TOKEN_LT, "<", line}; }
    if (current == '>') { pos++; return {TOKEN_GT, ">", line}; }

    if (current == '"') {
        int startLine = line;
        std::string strValue = "\""; 
        pos++; 
        while (pos < code.length() && code[pos] != '"') {
            if (code[pos] == '\n') line++;
            strValue += code[pos++];
        }
        if (pos >= code.length()) {
            return {TOKEN_ERROR, "String sin cerrar en linea " + std::to_string(startLine), startLine};
        }
        strValue += '"'; 
        pos++; 
        return {TOKEN_STRING, strValue, line};
    }

    if (isdigit(current)) {
        std::string num = "";
        while (pos < code.length() && isdigit(code[pos])) num += code[pos++];
        return {TOKEN_NUMBER, num, line};
    }

    if (isalpha(current) || current == '_') {
        std::string word = "";
        while (pos < code.length() && (isalnum(code[pos]) || code[pos] == '_')) word += code[pos++];
        
        static const std::unordered_map<std::string, TokenType> keywords = {
            {"var", TOKEN_VAR}, {"if", TOKEN_IF}, {"else", TOKEN_ELSE},
            {"repeat", TOKEN_REPEAT}, {"exists", TOKEN_EXISTS},
            {"move_file", TOKEN_CMD_MOVE}, {"copy_file", TOKEN_CMD_COPY},
            {"create_file", TOKEN_CMD_CREATE}, {"delete_file", TOKEN_CMD_DELETE},
            {"print", TOKEN_CMD_PRINT}, {"wait", TOKEN_CMD_WAIT}
        };
        auto it = keywords.find(word);
        if (it != keywords.end()) return {it->second, word, line};
        return {TOKEN_ID, word, line};
    }

    pos++;
    return {TOKEN_ERROR, "Caracter inesperado '" + std::string(1, current) + "' en linea " + std::to_string(line), line};
}