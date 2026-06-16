// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_COMPILER_CODE_H
#define BIBBLEVM_COMPILER_CODE_H 1

#include <cstdint>

namespace bibblevm {
    struct Code {
        void* mc; // for those who just joined, mc is slang for machine code
        size_t size;
    };
}

#endif // BIBBLEVM_COMPILER_JIT_CODE_H
