// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_CORE_MODULE_INSTRUCTION_H
#define BIBBLEVM_CORE_MODULE_INSTRUCTION_H 1

#include "BibbleVM/api.h"

#include <cstdint>

namespace bibblevm::module {
    struct Instruction {
        uint8_t bytes[4];

        constexpr Instruction(uint8_t opcode, uint8_t a, uint8_t b, uint8_t c)
            : bytes{opcode, a, b, c} {}

        constexpr Instruction(const uint8_t* bytes)
            : bytes{bytes[0], bytes[1], bytes[2], bytes[3]} {}

        constexpr uint8_t getOpcode() const { return bytes[0]; }
        constexpr uint8_t getArgumentA() const { return bytes[1]; }
        constexpr uint8_t getArgumentB() const { return bytes[2]; }
        constexpr uint8_t getArgumentC() const { return bytes[3]; }
    };

    class BIBBLEVM_EXPORT InstructionStream {
    public:
        InstructionStream(const uint8_t* bytes, size_t size);

        size_t decodeInstructionCount() const;
        Instruction fetchOne();

        void reset();

    private:
        const uint8_t* mBytes;
        const uint8_t* mBytesEnd;
        const uint8_t* mCurrent;
    };
}

#endif //BIBBLEVM_CORE_MODULE_INSTRUCTION_H