// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_COMPILER_COMPILATION_UNIT_H
#define BIBBLEVM_COMPILER_COMPILATION_UNIT_H 1

#include "BibbleVM/compiler/code.h"

namespace bibblevm {
    enum class CompilationState {
        None,
        Compiling,
        Compiled,
        Failed
    };

    struct CompilationUnit {
        CompilationState state = CompilationState::None;

        Code* baseline = nullptr;
    };
}

#endif // BIBBLEVM_COMPILER_COMPILATION_UNIT_H
