// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_COMPILER_CODE_H
#define BIBBLEVM_COMPILER_CODE_H 1

#include <cstdint>

namespace bibblevm {
    struct Code {
        size_t size;

        uint8_t mc[];
    };
}

#endif // BIBBLEVM_COMPILER_JIT_CODE_H
