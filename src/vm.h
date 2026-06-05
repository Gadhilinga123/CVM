#pragma once
#include "compiler.h"

class VM {
    std::vector<int> stack;      // the operand stack
    std::vector<int> variables;  // storage for all variables

    void push(int v);
    int  pop();

public:
    void execute(const Bytecode& bc);
};