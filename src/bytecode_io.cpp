#include "bytecode_io.h"
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <iomanip>

// ── helpers ──────────────────────────────────────────────────────────────────

// Write a 4-byte little-endian uint32_t
static void writeU32(std::ostream& out, uint32_t v) {
    out.put(static_cast<char>( v        & 0xFF));
    out.put(static_cast<char>((v >>  8) & 0xFF));
    out.put(static_cast<char>((v >> 16) & 0xFF));
    out.put(static_cast<char>((v >> 24) & 0xFF));
}

// Read a 4-byte little-endian uint32_t
static uint32_t readU32(std::istream& in) {
    uint8_t b[4];
    in.read(reinterpret_cast<char*>(b), 4);
    if (!in) throw std::runtime_error("Unexpected end of bytecode file.");
    return static_cast<uint32_t>(b[0])
         | static_cast<uint32_t>(b[1]) <<  8
         | static_cast<uint32_t>(b[2]) << 16
         | static_cast<uint32_t>(b[3]) << 24;
}

// Write a 4-byte little-endian int32_t (operand)
static void writeI32(std::ostream& out, int32_t v) {
    writeU32(out, static_cast<uint32_t>(v));
}

// Read a 4-byte little-endian int32_t (operand)
static int32_t readI32(std::istream& in) {
    return static_cast<int32_t>(readU32(in));
}

// ── save ─────────────────────────────────────────────────────────────────────

bool saveBytecode(const Bytecode& bc, const std::string& path) {
    std::ofstream out(path, std::ios::binary);
    if (!out.is_open()) {
        std::cerr << "Error: Cannot write to '" << path << "'\n";
        return false;
    }

    // Magic + version
    out.write("CVMB", 4);
    out.put(0x01);  // version 1

    // Variable table
    uint32_t varCount = static_cast<uint32_t>(bc.varTable.size());
    writeU32(out, varCount);
    for (const auto& name : bc.varTable) {
        uint32_t len = static_cast<uint32_t>(name.size());
        writeU32(out, len);
        out.write(name.data(), len);
    }

    // Instructions
    uint32_t instrCount = static_cast<uint32_t>(bc.instructions.size());
    writeU32(out, instrCount);
    for (const auto& instr : bc.instructions) {
        out.put(static_cast<char>(static_cast<uint8_t>(instr.op)));
        writeI32(out, static_cast<int32_t>(instr.operand));
    }

    return out.good();
}

// ── load ─────────────────────────────────────────────────────────────────────

Bytecode loadBytecode(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in.is_open())
        throw std::runtime_error("Cannot open bytecode file: '" + path + "'");

    // Verify magic
    char magic[4];
    in.read(magic, 4);
    if (!in || magic[0]!='C' || magic[1]!='V' || magic[2]!='M' || magic[3]!='B')
        throw std::runtime_error("Not a valid .cvmb file (bad magic bytes).");

    // Verify version
    uint8_t version = static_cast<uint8_t>(in.get());
    if (version != 0x01)
        throw std::runtime_error("Unsupported .cvmb version: "
                                 + std::to_string(version));

    Bytecode bc;

    // Variable table
    uint32_t varCount = readU32(in);
    bc.varTable.resize(varCount);
    for (uint32_t i = 0; i < varCount; ++i) {
        uint32_t len = readU32(in);
        bc.varTable[i].resize(len);
        in.read(bc.varTable[i].data(), len);
        if (!in) throw std::runtime_error("Truncated variable name in .cvmb.");
    }

    // Instructions
    uint32_t instrCount = readU32(in);
    bc.instructions.resize(instrCount);
    for (uint32_t i = 0; i < instrCount; ++i) {
        uint8_t opByte = static_cast<uint8_t>(in.get());
        int32_t operand = readI32(in);
        bc.instructions[i] = { static_cast<Opcode>(opByte),
                                static_cast<int>(operand) };
        if (!in) throw std::runtime_error("Truncated instruction in .cvmb.");
    }

    return bc;
}

// ── disassemble ───────────────────────────────────────────────────────────────

static const char* opName(Opcode op) {
    switch (op) {
        case Opcode::PUSH:           return "PUSH";
        case Opcode::LOAD:           return "LOAD";
        case Opcode::STORE:          return "STORE";
        case Opcode::ADD:            return "ADD";
        case Opcode::SUB:            return "SUB";
        case Opcode::MUL:            return "MUL";
        case Opcode::DIV:            return "DIV";
        case Opcode::EQ:             return "EQ";
        case Opcode::LT:             return "LT";
        case Opcode::PRINT:          return "PRINT";
        case Opcode::INPUT:          return "INPUT";
        case Opcode::JUMP:           return "JUMP";
        case Opcode::JUMP_IF_FALSE:  return "JUMP_IF_FALSE";
        case Opcode::HALT:           return "HALT";
        default:                     return "???";
    }
}

void disassemble(const Bytecode& bc) {
    std::cout << "====== Disassembly ======\n";
    std::cout << "Variables (" << bc.varTable.size() << "):\n";
    for (size_t i = 0; i < bc.varTable.size(); ++i)
        std::cout << "  [" << i << "] " << bc.varTable[i] << "\n";

    std::cout << "\nInstructions (" << bc.instructions.size() << "):\n";
    for (size_t i = 0; i < bc.instructions.size(); ++i) {
        const auto& ins = bc.instructions[i];
        std::cout << "  " << std::setw(4) << i << "  "
                  << std::left << std::setw(16) << opName(ins.op);

        // Print operand with context where it adds meaning
        switch (ins.op) {
            case Opcode::PUSH:
                std::cout << ins.operand;
                break;
            case Opcode::LOAD:
            case Opcode::STORE:
            case Opcode::INPUT:
                if (ins.operand < (int)bc.varTable.size())
                    std::cout << ins.operand
                              << "  ; " << bc.varTable[ins.operand];
                else
                    std::cout << ins.operand;
                break;
            case Opcode::JUMP:
            case Opcode::JUMP_IF_FALSE:
                std::cout << "→ " << ins.operand;
                break;
            default:
                break;
        }
        std::cout << "\n";
    }
    std::cout << "=========================\n";
}
