#include "compiler.h"

// Look up variable by name; if not seen before, add it to the table
int Compiler::getOrAddVar(const std::string& name) {
    for (int i = 0; i < (int)bc.varTable.size(); i++)
        if (bc.varTable[i] == name) return i;
    bc.varTable.push_back(name);
    return bc.varTable.size() - 1;
}

void Compiler::compileBlock(const std::vector<NodePtr>& block) {
    for (auto& stmt : block)
        compileNode(stmt.get());
}

void Compiler::compileNode(const ASTNode* node) {

    // Number literal → PUSH that value
    if (auto* n = dynamic_cast<const NumberNode*>(node)) {
        bc.instructions.push_back({Opcode::PUSH, n->value});
    }

    else if (auto* n = dynamic_cast<const BoolNode*>(node)) {
        bc.instructions.push_back({Opcode::PUSH, n->value ? 1 : 0});
    }

    else if (auto* n = dynamic_cast<const VarNode*>(node)) {
        bc.instructions.push_back({Opcode::LOAD, getOrAddVar(n->name)});
    }

    else if (auto* n = dynamic_cast<const BinaryOpNode*>(node)) {
        compileNode(n->left.get());    // compile left → pushes left value
        compileNode(n->right.get());   // compile right → pushes right value
        // Now stack has: [..., leftVal, rightVal]
        if      (n->op == "+")  bc.instructions.push_back({Opcode::ADD});
        else if (n->op == "-")  bc.instructions.push_back({Opcode::SUB});
        else if (n->op == "*")  bc.instructions.push_back({Opcode::MUL});
        else if (n->op == "/")  bc.instructions.push_back({Opcode::DIV});
        else if (n->op == "==") bc.instructions.push_back({Opcode::EQ});
        else if (n->op == "<")  bc.instructions.push_back({Opcode::LT});
    }

    else if (auto* n = dynamic_cast<const AssignNode*>(node)) {
        compileNode(n->expr.get());    // compile the right-hand side
        bc.instructions.push_back({Opcode::STORE, getOrAddVar(n->name)});
    }

    else if (auto* n = dynamic_cast<const PrintNode*>(node)) {
        compileNode(n->expr.get());    // compile expression, result on stack
        bc.instructions.push_back({Opcode::PRINT});
    }

    else if (auto* n = dynamic_cast<const InputNode*>(node)) {
        bc.instructions.push_back({Opcode::INPUT, getOrAddVar(n->varName)});
    }

    else if (auto* n = dynamic_cast<const IfNode*>(node)) {
        compileNode(n->condition.get());   // evaluate condition → 0 or 1 on stack

        // Emit JUMP_IF_FALSE with placeholder target (we don't know where yet)
        int jifIdx = bc.instructions.size();
        bc.instructions.push_back({Opcode::JUMP_IF_FALSE, 0});

        // Compile the then-block
        compileBlock(n->thenBlock);

        if (!n->elseBlock.empty()) {
            // Need an unconditional jump at end of then-block to skip else
            int jumpIdx = bc.instructions.size();
            bc.instructions.push_back({Opcode::JUMP, 0});

            // Now we know where else starts — patch the JUMP_IF_FALSE
            bc.instructions[jifIdx].operand = bc.instructions.size();

            compileBlock(n->elseBlock);

            // Now we know where else ends — patch the JUMP
            bc.instructions[jumpIdx].operand = bc.instructions.size();
        } else {
            // No else — JUMP_IF_FALSE jumps to right after the then-block
            bc.instructions[jifIdx].operand = bc.instructions.size();
        }
    }

    else if (auto* n = dynamic_cast<const WhileNode*>(node)) {
        int loopStart = bc.instructions.size();  // remember where loop begins

        compileNode(n->condition.get());          // evaluate condition

        int jifIdx = bc.instructions.size();
        bc.instructions.push_back({Opcode::JUMP_IF_FALSE, 0}); // exit if false

        compileBlock(n->body);

        // Jump back to top of loop
        bc.instructions.push_back({Opcode::JUMP, loopStart});

        // Patch the exit jump
        bc.instructions[jifIdx].operand = bc.instructions.size();
    }

    else if (auto* n = dynamic_cast<const ProgramNode*>(node)) {
        compileBlock(n->statements);
        bc.instructions.push_back({Opcode::HALT});
    }
}

Bytecode Compiler::compile(const ProgramNode* program) {
    compileNode(program);
    return std::move(bc);
}