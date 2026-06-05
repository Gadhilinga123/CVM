#pragma once
#include "parser.h"
#include <cstdint>

enum class Opcode : uint8_t {
    PUSH,           // Push a constant integer onto the stack
    LOAD,           // Load a variable's value onto the stack
    STORE,          // Pop stack top and save to a variable
    ADD,            // Pop two values, push their sum
    SUB,            // Pop two values, push their difference
    MUL,            // Pop two values, push their product
    DIV,            // Pop two values, push their quotient
    EQ,             // Pop two values, push 1 if equal else 0
    LT,             // Pop two values, push 1 if left < right else 0
    PRINT,          // Pop top of stack and print it
    INPUT,          // Read integer from stdin, store in variable
    JUMP,           // Jump to instruction at given index unconditionally
    JUMP_IF_FALSE,  // Pop top; if it's 0, jump to given index
    HALT            // Stop execution
};

struct Instruction {
    Opcode op;
    int operand = 0;   // extra data the opcode needs
};

struct Bytecode {
    std::vector<Instruction> instructions;   // the instruction list
    std::vector<std::string> varTable;       // maps variable index → name
};

class Compiler {
    Bytecode bc;
    int getOrAddVar(const std::string& name);
    void compileNode(const ASTNode* node);
    void compileBlock(const std::vector<NodePtr>& block);
public:
    Bytecode compile(const ProgramNode* program);
};