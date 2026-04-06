// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/core/module/instruction.h"

namespace bibblevm::module {
    InstructionStream::InstructionStream(const uint8_t* bytes, size_t size)
        : mBytes(bytes)
        , mBytesEnd(bytes + size)
        , mCurrent(bytes) {}

    size_t InstructionStream::decodeInstructionCount() const {
        return (mBytesEnd - mCurrent) / 4; // this is probably right
    }

    Instruction InstructionStream::fetchOne() {
        if (mCurrent >= mBytesEnd) return {0, 0, 0, 0}; // TODO: proper error
        const uint8_t* current = mCurrent;
        mCurrent += 4;
        return current;
    }

    void InstructionStream::reset() {
        mCurrent = mBytes;
    }
}
