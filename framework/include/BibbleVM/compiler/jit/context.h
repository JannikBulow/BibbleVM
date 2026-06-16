// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_COMPILER_JIT_CONTEXT_H
#define BIBBLEVM_COMPILER_JIT_CONTEXT_H 1

#include "BibbleVM/compiler/code.h"
#include "BibbleVM/compiler/compiler.h"

#include <cstdint>

namespace bibblevm::jit {
    struct Context {
        enum State : uint8_t {
            Queued,
            Compiling,
            Compiled,
            Failed
        };

        State state = Queued;
        ::bibblevm::Compiler compiler;

        Code* code = nullptr;
    };
}

#endif // BIBBLEVM_COMPILER_JIT_CONTEXT_H
