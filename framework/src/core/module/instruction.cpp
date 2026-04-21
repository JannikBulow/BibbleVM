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

        opcodeutils::PrefixState prefix{};

        Opcode opcode = *current++;
        while ((opcode & PREFIX_MASK) == PREFIX_MASK) {
            switch (static_cast<Prefixes>(opcode)) {
                case WIDE_OPERAND0:
                    prefix.wideOperand0 = true;
                    break;
                case WIDE_OPERAND1:
                    prefix.wideOperand1 = true;
                    break;
                case WIDE_OPERAND2:
                    prefix.wideOperand2 = true;
                    break;
                case WIDE_OPERAND3:
                    prefix.wideOperand3 = true;
                    break;
                case HUGE_IMMEDIATE:
                    prefix.hugeImmediate = true;
                    break;
                case GIGANTIC_IMMEDIATE:
                    prefix.giganticImmediate = true;
                    break;
            }
            opcode = *current++;
        }

        size_t length = opcodeutils::GetFixedLength(opcode, prefix);

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
