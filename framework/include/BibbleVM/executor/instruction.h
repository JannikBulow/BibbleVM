// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_INSTRUCTION_H
#define BIBBLEVM_INSTRUCTION_H 1

#include "BibbleVM/executor/interpreter.h"

#include "BibbleVM/util/list.h"

#include "BibbleVM/api.h"

#include <cstdint>

namespace bibblevm::executor {
    // Pre-decoded instruction. Trade memory for 0.0002% faster dispatch
    struct BIBBLEVM_EXPORT Instruction {
        Interpreter interpreter;
        uint8_t a, b, c;
    };

    using InstructionList = List<Instruction>;
}

#endif // BIBBLEVM_INSTRUCTION_H
