#pragma once
#include "compiler.h"
#include <string>

// ---------------------------------------------------------------
//  CVM++ Bytecode File Format  (.cvmb)
// ---------------------------------------------------------------
//  Offset  Size     Field
//  ------  -------  -------------------------------------------
//  0       4 bytes  Magic: 'C','V','M','B'
//  4       1 byte   Version: 0x01
//  5       4 bytes  VarCount  (uint32_t, little-endian)
//  9       …        VarTable entries:
//                     [4-byte name-length][name chars]  × VarCount
//  …       4 bytes  InstrCount (uint32_t, little-endian)
//  …       5 bytes  Instruction × InstrCount:
//                     [1-byte opcode][4-byte signed operand]
// ---------------------------------------------------------------

// Write bytecode to a .cvmb file. Returns true on success.
bool saveBytecode(const Bytecode& bc, const std::string& path);

// Read a .cvmb file back into a Bytecode struct.
// Throws std::runtime_error on format or IO errors.
Bytecode loadBytecode(const std::string& path);

// Pretty-print a disassembly of the bytecode to stdout.
void disassemble(const Bytecode& bc);
