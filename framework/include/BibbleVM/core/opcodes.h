// Copyright 2026 JesusTouchMe

#ifndef BIBBLEVM_OPCODES_H
#define BIBBLEVM_OPCODES_H 1

#include "BibbleVM/api.h"

#include <cstdint>

namespace bibblevm {
    enum Opcodes : uint8_t {
        NOP = 0x0,

        MOV = 0x1,

        ADD = 0x2,
        SUB = 0x3,

        RETURN = 0x90,
    };
}

#endif //BIBBLEVM_OPCODES_H