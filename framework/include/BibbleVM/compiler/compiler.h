// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_COMPILER_COMPILER_H
#define BIBBLEVM_COMPILER_COMPILER_H 1

#include "BibbleVM/core/executor/function.h"

#include <asmjit/asmjit.h>

#include <vector>

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
        explicit Compiler(executor::Function* function);

        Code* compile(VM& vm, CompileOptions options);

    private:
        executor::Function* mFunction;
        executor::Instruction* mInstructionsBegin;
        executor::Instruction* mInstructionsEnd;

        x86::Assembler* mAsm = nullptr;
        std::vector<Label> mLabels;

        bool compileInstruction(VM& vm, executor::Instruction* instruction);

        x86::Mem getIsObjectAddress(uint16_t vregIndex);
        x86::Mem getValueAddress(uint16_t vregIndex);
    };
}

#endif // BIBBLEVM_COMPILER_COMPILER_H
