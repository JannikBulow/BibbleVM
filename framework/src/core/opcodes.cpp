// Copyright 2026 JesusTouchMe

#include "BibbleVM/core/opcodes.h"

namespace bibblevm {
    InstructionStream::InstructionStream(const uint8_t* bytes, size_t size)
        : mBytes(bytes)
        , mBytesEnd(bytes + size)
        , mCurrent(bytes) {}

    RawInstruction InstructionStream::fetchOne() {
        if (mCurrent >= mBytesEnd) return {0, 0, 0, 0}; // TODO: proper error
        const uint8_t* current = mCurrent;
        mCurrent += 4;
        return current;
    }

    void InstructionStream::reset() {
        mCurrent = mBytes;
    }
}
