// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_CORE_MODULE_INSTRUCTION_H
#define BIBBLEVM_CORE_MODULE_INSTRUCTION_H 1

#include "BibbleVM/core/opcodes.h"

#include "BibbleVM/api.h"

#include <cstdint>
#include <optional>

namespace bibblevm::module {
    // This type must be hashable in case it's changed in the future
    using BytecodeStream = const uint8_t*;

    struct Instruction {
        opcodeutils::PrefixState prefixState;
        Opcode opcode;
        BytecodeStream argsBegin;
        BytecodeStream argsEnd;
    };

    size_t DecodeInstructionCount(BytecodeStream begin, BytecodeStream end);

    std::optional<Instruction> FetchInstruction(BytecodeStream& current, BytecodeStream end);
}

#endif //BIBBLEVM_CORE_MODULE_INSTRUCTION_H