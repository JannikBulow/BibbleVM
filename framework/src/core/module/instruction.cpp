// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/core/module/instruction.h"

namespace bibblevm::module {
    size_t DecodeInstructionCount(BytecodeStream begin, BytecodeStream end) {
        BytecodeStream current = begin;
        size_t count = 0;

        while (true) {
            if (!FetchInstruction(begin, end, current).has_value()) break;
            count++;
        }

        return count;
    }

    std::optional<Instruction> FetchInstruction(BytecodeStream begin, BytecodeStream end, BytecodeStream& current) {
        if (current >= end) return std::nullopt;

        BytecodeStream start = current;

        Opcode opcode = *current++;
        size_t length = opcodeutils::GetFixedLength(opcode, TODO);

        if (length == 0) {
            return std::nullopt; // TODO: decode variable-length instructions
        }

        if (start + length > end) return std::nullopt;

        BytecodeStream argsBegin = current;
        BytecodeStream argsEnd = start + length;

        current = start + length;

        return Instruction(opcode, argsBegin, argsEnd);
    }
}
