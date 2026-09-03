#include "ast.h"
#include <fstream>
#include <thread>
#include <chrono>
#include <filesystem>
#include <stdexcept>

static std::string resolve(const std::string& expr, const SymbolTable& symTable) {
    // remueve comillas de la expresion directa
    if (expr.length() >= 2 && expr.front() == '"' && expr.back() == '"') {
        return expr.substr(1, expr.length() - 2); 
    }
    
    // busca valor si es variable
    auto it = symTable.find(expr);
    if (it != symTable.end()) {
        const std::string& val = it->second;
        if (val.length() >= 2 && val.front() == '"' && val.back() == '"') {
            return val.substr(1, val.length() - 2);
        }
        return val;
    }
    
    // si es un numero o plano
    return expr; 
}


VarDeclNode::VarDeclNode(const std::string& id, const std::string& expr) : identifier(id), expression(expr) {}
void VarDeclNode::execute(SymbolTable& symTable) {
    if (symTable.find(identifier) != symTable.end()) {
        throw std::runtime_error("Semantica: La variable '" + identifier + "' ya ha sido declarada en este entorno.");
    }
    symTable[identifier] = expression; 
}

AssignmentNode::AssignmentNode(const std::string& id, const std::string& expr) : identifier(id), expression(expr) {}
void AssignmentNode::execute(SymbolTable& symTable) {
    if (symTable.find(identifier) == symTable.end()) {
        throw std::runtime_error("Semantica: Variable '" + identifier + "' no declarada.");
    }
    symTable[identifier] = expression;
}

CommandCallNode::CommandCallNode(const std::string& cmd, std::vector<std::string> args) 
    : commandName(cmd), arguments(std::move(args)) {}

void CommandCallNode::execute(SymbolTable& symTable) {
    std::vector<std::string> realArgs;
    for (const auto& arg : arguments) {
        realArgs.push_back(resolve(arg, symTable));
    }

    if (commandName == "create_file") {
        if (realArgs.empty()) 
            throw std::runtime_error("Semantica: 'create_file' requiere 1 argumento.");
        std::ofstream f(realArgs[0]);
        if (!f.is_open()) 
            throw std::runtime_error("Semantica: No se pudo crear '" + realArgs[0] + "'.");
        std::cout << "[SISTEMA] Archivo '" << realArgs[0] << "' creado.\n";
    } 
    else if (commandName == "delete_file") {
        if (realArgs.empty()) 
            throw std::runtime_error("Semantica: 'delete_file' requiere 1 argumento.");
        if (std::remove(realArgs[0].c_str()) != 0)
            throw std::runtime_error("Semantica: No se pudo eliminar '" + realArgs[0] + "'.");
        std::cout << "[SISTEMA] Archivo '" << realArgs[0] << "' eliminado.\n";
    } 
    else if (commandName == "move_file") {
        if (realArgs.size() < 2) 
            throw std::runtime_error("Semantica: 'move_file' requiere 2 argumentos.");
        std::rename(realArgs[0].c_str(), realArgs[1].c_str());
        std::cout << "[SISTEMA] Movido de '" << realArgs[0] << "' a '" << realArgs[1] << "'.\n";
    }
    else if (commandName == "copy_file") {
        if (realArgs.size() < 2) 
            throw std::runtime_error("Semantica: 'copy_file' requiere 2 argumentos.");
        std::filesystem::copy(realArgs[0], realArgs[1], std::filesystem::copy_options::overwrite_existing);
        std::cout << "[SISTEMA] Copiado de '" << realArgs[0] << "' a '" << realArgs[1] << "'.\n";
    }
    else if (commandName == "print") {
        if (realArgs.empty()) 
            throw std::runtime_error("Semantica: 'print' requiere 1 argumento.");
        std::cout << "[CONSOLA] " << realArgs[0] << "\n";
    }
    else if (commandName == "wait") {
        if (realArgs.empty()) 
            throw std::runtime_error("Semantica: 'wait' requiere 1 argumento.");
        try {
            int ms = std::stoi(realArgs[0]);
            std::this_thread::sleep_for(std::chrono::milliseconds(ms));
        } catch (...) {
            throw std::runtime_error("Semantica: 'wait' esperaba un numero valido.");
        }
    }
}

ProgramNode::ProgramNode(std::vector<std::unique_ptr<ASTNode>> stmts) : statements(std::move(stmts)) {}
void ProgramNode::execute(SymbolTable& symTable) {
    for (auto& stmt : statements) stmt->execute(symTable);
}

RepeatNode::RepeatNode(const std::string& iters, std::unique_ptr<ASTNode> bodyNode) 
    : iterations(iters), body(std::move(bodyNode)) {}
void RepeatNode::execute(SymbolTable& symTable) {
    std::string res = resolve(iterations, symTable);
    try {
        int repeticiones = std::stoi(res);
        for (int i = 0; i < repeticiones; i++) body->execute(symTable);
    } catch (...) {
        throw std::runtime_error("Semantica: 'repeat' esperaba un numero, obtuvo '" + res + "'.");
    }
}

ConditionNode::ConditionNode(const std::string& op, const std::string& left, const std::string& right) 
    : op(op), leftExpr(left), rightExpr(right) {}
bool ConditionNode::evaluate(SymbolTable& symTable) {
    std::string leftVal = resolve(leftExpr, symTable);
    
    if (op == "exists") {
        std::ifstream f(leftVal.c_str());
        return f.good();
    } else {
        std::string rightVal = resolve(rightExpr, symTable);
        if (op == "==") return leftVal == rightVal;
        if (op == "!=") return leftVal != rightVal;
        
        try {
            int lNum = std::stoi(leftVal);
            int rNum = std::stoi(rightVal);
            if (op == "<") return lNum < rNum;
            if (op == ">") return lNum > rNum;
        } catch (...) { return false; }
    }
    return false;
}

IfNode::IfNode(std::unique_ptr<ConditionNode> cond, std::unique_ptr<ASTNode> thenB, std::unique_ptr<ASTNode> elseB)
    : condition(std::move(cond)), thenBody(std::move(thenB)), elseBody(std::move(elseB)) {}

void IfNode::execute(SymbolTable& symTable) {
    if (condition->evaluate(symTable)) {
        thenBody->execute(symTable);
    } else if (elseBody) {
        elseBody->execute(symTable);
    }
}