// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/core/module/instruction.h"

#include <utility>

namespace bibblevm::module {
    size_t DecodeInstructionCount(BytecodeStream begin, BytecodeStream end) {
        BytecodeStream current = begin;
        size_t count = 0;

        while (current != end) {
            count++;
            std::ignore = FetchInstruction(begin, end, current);
        }

        return count;
    }

    Instruction FetchInstruction(BytecodeStream begin, BytecodeStream end, BytecodeStream& current) {

    }
}
