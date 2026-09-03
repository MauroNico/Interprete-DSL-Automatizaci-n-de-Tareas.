#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <iostream>

using SymbolTable = std::unordered_map<std::string, std::string>;

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void execute(SymbolTable& symTable) = 0; 
};

class VarDeclNode : public ASTNode {
    std::string identifier;
    std::string expression;
public:
    VarDeclNode(const std::string& id, const std::string& expr);
    void execute(SymbolTable& symTable) override;
};


class AssignmentNode : public ASTNode {
    std::string identifier;
    std::string expression;
public:
    AssignmentNode(const std::string& id, const std::string& expr);
    void execute(SymbolTable& symTable) override;
};

class CommandCallNode : public ASTNode {
    std::string commandName;
    std::vector<std::string> arguments;
public:
    CommandCallNode(const std::string& cmd, std::vector<std::string> args);
    void execute(SymbolTable& symTable) override;
};

class ProgramNode : public ASTNode {
    std::vector<std::unique_ptr<ASTNode>> statements;
public:
    ProgramNode(std::vector<std::unique_ptr<ASTNode>> stmts);
    void execute(SymbolTable& symTable) override;
};

class RepeatNode : public ASTNode {
    std::string iterations;
    std::unique_ptr<ASTNode> body;
public:
    RepeatNode(const std::string& iters, std::unique_ptr<ASTNode> bodyNode);
    void execute(SymbolTable& symTable) override;
};

class ConditionNode {
    std::string op;
    std::string leftExpr;
    std::string rightExpr;
public:
    ConditionNode(const std::string& op, const std::string& left, const std::string& right);
    bool evaluate(SymbolTable& symTable);
};

class IfNode : public ASTNode {
    std::unique_ptr<ConditionNode> condition;
    std::unique_ptr<ASTNode> thenBody;
    std::unique_ptr<ASTNode> elseBody;
public:
    IfNode(std::unique_ptr<ConditionNode> cond, std::unique_ptr<ASTNode> thenB, std::unique_ptr<ASTNode> elseB);
    void execute(SymbolTable& symTable) override;
};

#endif