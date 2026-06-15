// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_JIT_CONTEXT_H
#define BIBBLEVM_JIT_CONTEXT_H 1

#include "BibbleVM/compiler/jit/code.h"

#include <cstdint>

namespace bibblevm::jit {
    struct Context {
        enum State : uint8_t {
            Queued,
            Compiling,
            Compiled
        };

        State state = Queued;

        Code* code = nullptr;
    };
}

#endif // BIBBLEVM_JIT_CONTEXT_H
