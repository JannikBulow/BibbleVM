// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_COMPILER_COMPILER_H
#define BIBBLEVM_COMPILER_COMPILER_H 1

#include "BibbleVM/core/executor/function.h"

namespace bibblevm::compiler {
    enum class OptLevel {
        O0,
    };

    struct CompileOptions {
        OptLevel opt;
    };

    class Compiler {
    public:
        Code* compile(VM& vm, executor::Function* function, CompileOptions options);
    };
}

#endif // BIBBLEVM_COMPILER_COMPILER_H
