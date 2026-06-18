// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_COMPILER_COMPILER_H
#define BIBBLEVM_COMPILER_COMPILER_H 1

#include "BibbleVM/core/executor/function.h"

#include <asmjit/asmjit.h>

namespace bibblevm::compiler {
    using namespace asmjit;

    enum class OptLevel {
        O0,
    };

    struct CompileOptions {
        OptLevel opt;
    };

    class BIBBLEVM_EXPORT Compiler {
    public:
        Code* compile(VM& vm, executor::Function* function, CompileOptions options);

    private:
        x86::Mem getIsObjectAddress(uint16_t vregIndex);
        x86::Mem getValueAddress(uint16_t vregIndex);
    };
}

#endif // BIBBLEVM_COMPILER_COMPILER_H
