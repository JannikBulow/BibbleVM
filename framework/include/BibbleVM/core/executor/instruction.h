// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_CORE_INSTRUCTION_H
#define BIBBLEVM_CORE_INSTRUCTION_H 1

#include "BibbleVM/core/executor/interpreter.h"

#include "BibbleVM/api.h"

#include <cstdint>

namespace bibblevm::executor {
    //TODO: optimize size later
    struct InstructionArguments {
        uint64_t a;
        uint64_t b;
        uint64_t c;
        uint64_t d;
    };

    // Pre-decoded instruction. Trade memory for 0.0002% faster dispatch
    struct Instruction {
        Opcode opcode;
        Interpreter interpreter;
        InstructionArguments args;
    };
}

#endif // BIBBLEVM_CORE_INSTRUCTION_H
