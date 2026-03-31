// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_INSTRUCTION_H
#define BIBBLEVM_INSTRUCTION_H 1

#include "BibbleVM/executor/interpreter.h"

#include "BibbleVM/api.h"

#include <cstdint>

namespace bibblevm::executor {
    union InstructionArguments {
        struct { uint8_t a, b, c; } generic;
        struct { uint8_t a; uint16_t bc; } extBC;
        struct { uint16_t ab; uint8_t c; } extAB;
    };

    // Pre-decoded instruction. Trade memory for 0.0002% faster dispatch
    struct Instruction {
        Interpreter interpreter;
        InstructionArguments args;
    };
}

#endif // BIBBLEVM_INSTRUCTION_H
