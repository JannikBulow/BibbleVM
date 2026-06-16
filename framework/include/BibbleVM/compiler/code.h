// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_COMPILER_JIT_CODE_H
#define BIBBLEVM_COMPILER_JIT_CODE_H 1

#include <cstddef>

namespace bibblevm {
    struct Code {
        void* ptr;
        size_t size;
    };
}

#endif // BIBBLEVM_COMPILER_JIT_CODE_H
