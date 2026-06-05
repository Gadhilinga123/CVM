#pragma once
#include "lexer.h"
#include <memory>
#include <vector>

// Base class for all AST nodes
struct ASTNode {
    virtual ~ASTNode() = default;
};
using NodePtr = std::unique_ptr<ASTNode>;

// A literal integer: 42, 0, 100
struct NumberNode : ASTNode {
    int value;
    NumberNode(int v) : value(v) {}
};

// A literal boolean: true or false
struct BoolNode : ASTNode {
    bool value;
    BoolNode(bool v) : value(v) {}
};

// A variable reference: x, count, total
struct VarNode : ASTNode {
    std::string name;
    VarNode(std::string n) : name(n) {}
};

// An arithmetic or comparison expression: left OP right
struct BinaryOpNode : ASTNode {
    std::string op;        // "+", "-", "*", "/", "==", "<"
    NodePtr left, right;
    BinaryOpNode(std::string o, NodePtr l, NodePtr r)
        : op(o), left(std::move(l)), right(std::move(r)) {}
};

// Variable declaration or assignment: let x = expr; OR x = expr;
struct AssignNode : ASTNode {
    std::string name;
    NodePtr expr;
    AssignNode(std::string n, NodePtr e)
        : name(n), expr(std::move(e)) {}
};

// print(expr);
struct PrintNode : ASTNode {
    NodePtr expr;
    PrintNode(NodePtr e) : expr(std::move(e)) {}
};

// input(varName);
struct InputNode : ASTNode {
    std::string varName;
    InputNode(std::string n) : varName(n) {}
};

// if (condition) { ... } else { ... }
struct IfNode : ASTNode {
    NodePtr condition;
    std::vector<NodePtr> thenBlock;
    std::vector<NodePtr> elseBlock;   // empty if no else
};

// while (condition) { ... }
struct WhileNode : ASTNode {
    NodePtr condition;
    std::vector<NodePtr> body;
};

// The entire program — a list of statements
struct ProgramNode : ASTNode {
    std::vector<NodePtr> statements;
};

class Parser {
    std::vector<Token> tokens;
    size_t pos = 0;

    Token& peek();
    Token consume(TokenType expected);
    NodePtr parseStatement();
    NodePtr parseExpr();
    NodePtr parsePrimary();
    std::vector<NodePtr> parseBlock();

public:
    Parser(std::vector<Token> t) : tokens(std::move(t)) {}
    std::unique_ptr<ProgramNode> parse();
};