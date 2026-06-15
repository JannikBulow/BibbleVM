// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_COMPILER_COMPILER_H
#define BIBBLEVM_COMPILER_COMPILER_H 1

#include "BibbleVM/core/executor/function.h"

namespace bibblevm {
    class Compiler {
    public:
        void compile(VM& vm, executor::Function* function);
    };
}

#endif // BIBBLEVM_COMPILER_COMPILER_H
