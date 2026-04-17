// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_CORE_MODULE_INSTRUCTION_H
#define BIBBLEVM_CORE_MODULE_INSTRUCTION_H 1

#include "BibbleVM/core/opcodes.h"

#include "BibbleVM/api.h"

#include <cstdint>

namespace bibblevm::module {
    using BytecodeStream = const uint8_t*;

    struct Instruction {
        Opcode opcode;
        BytecodeStream argsBegin;
        BytecodeStream argsEnd;
    };

    size_t DecodeInstructionCount(BytecodeStream begin, BytecodeStream end);
    Instruction FetchInstruction(BytecodeStream begin, BytecodeStream end, BytecodeStream& current);
}

#endif //BIBBLEVM_CORE_MODULE_INSTRUCTION_H