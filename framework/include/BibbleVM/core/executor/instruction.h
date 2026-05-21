// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_CORE_INSTRUCTION_H
#define BIBBLEVM_CORE_INSTRUCTION_H 1

#include "BibbleVM/core/executor/interpreter.h"

#include "BibbleVM/api.h"

#include <cstdint>

namespace bibblevm::executor {
    // Pre-decoded instruction. Trade memory for 0.0002% faster dispatch
    struct Instruction {
        Interpreter interpreter;
        Opcode opcode;
        uint16_t a;
        uint16_t b;
        uint16_t c;
        uint64_t imm; // imm can also act as a d arg for an instruction that's 4 args
    };
}

#endif // BIBBLEVM_CORE_INSTRUCTION_H
