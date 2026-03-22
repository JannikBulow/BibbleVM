// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_INSTRUCTION_H
#define BIBBLEVM_INSTRUCTION_H 1

#include "BibbleVM/executor/interpreter.h"

#include "BibbleVM/api.h"

#include <cstdint>
#include <memory>

namespace bibblevm {
    // Pre-decoded instruction. Trade memory for 0.0002% faster dispatch
    struct BIBBLEVM_EXPORT Instruction {
        Interpreter interpreter;
        uint8_t A, B, C;
    };

    class BIBBLEVM_EXPORT InstructionList {
    public:
        InstructionList(std::unique_ptr<Instruction[]> instructions, size_t count)
            : mInstructions(std::move(instructions))
            , mCount(count) {}

        Instruction* get() const {
            return mInstructions.get();
        }

        size_t getCount() const {
            return mCount;
        }

    private:
        std::unique_ptr<Instruction[]> mInstructions;
        size_t mCount;
    };
}

#endif // BIBBLEVM_INSTRUCTION_H
