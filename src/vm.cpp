#include "vm.h"
#include <iostream>
#include <stdexcept>

void VM::push(int v) {
    stack.push_back(v);
}

int VM::pop() {
    if (stack.empty())
        throw std::runtime_error("Stack underflow!");
    int v = stack.back();
    stack.pop_back();
    return v;
}

void VM::execute(const Bytecode& bc) {
    // Set up variable storage — one slot per variable, all start at 0
    variables.assign(bc.varTable.size(), 0);
    size_t ip = 0;  // instruction pointer starts at first instruction

    while (ip < bc.instructions.size()) {
        const auto& instr = bc.instructions[ip];

        switch (instr.op) {

            case Opcode::PUSH:
                push(instr.operand);
                break;

            case Opcode::LOAD:
                push(variables[instr.operand]);
                break;
            
            case Opcode::STORE:
                variables[instr.operand] = pop();
                break;

            case Opcode::ADD: { int b = pop(), a = pop(); push(a + b); } break;
            case Opcode::SUB: { int b = pop(), a = pop(); push(a - b); } break;
            case Opcode::MUL: { int b = pop(), a = pop(); push(a * b); } break;
            case Opcode::DIV: { int b = pop(), a = pop(); push(a / b); } break;
            case Opcode::EQ:  { int b = pop(), a = pop(); push(a == b ? 1 : 0); } break;
            case Opcode::LT:  { int b = pop(), a = pop(); push(a <  b ? 1 : 0); } break;

            case Opcode::PRINT:
                std::cout << pop() << "\n";
                break;

            case Opcode::INPUT: {
                int val;
                std::cout << "Enter value: ";
                std::cin >> val;
                variables[instr.operand] = val;
                break;
            }

            case Opcode::JUMP:
                ip = instr.operand;
                continue;   // use 'continue' to skip the ip++ at the bottom

            case Opcode::JUMP_IF_FALSE:
                if (pop() == 0) {
                    ip = instr.operand;
                    continue;
                }
                break;

            case Opcode::HALT:
                return;
        }

        ip++;   // advance to next instruction (jumps use 'continue' to skip this)
    }
}