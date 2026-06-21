// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_COMPILER_COMPILER_H
#define BIBBLEVM_COMPILER_COMPILER_H 1

#include "BibbleVM/compiler/abi.h"

#include "BibbleVM/core/executor/function.h"

#include "BibbleVM/core/error.h"

#include <asmjit/asmjit.h>

#include <optional>
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

        x86::Builder* mAsm = nullptr;
        std::vector<Label> mCheckpoints;
        std::vector<Label> mLabels;

        bool compileInstruction(VM& vm, executor::Instruction* instruction);

        x86::Mem getIsObjectAddress(uint16_t vregIndex);
        x86::Mem getValueAddress(uint16_t vregIndex);

        void createPlatformCall(const std::vector<int>& neededRegisters, int returnRegister, void* function); // for now, this required manually moving arguments to the correct registers

        uintptr_t createCheckpoint();
        void bindCheckpoint(uintptr_t checkpoint);
        void createLeave(abi::LeaveReason reason, bool generateCheckpoint); // assumes exit1 and exit2 are populated before call

        void createError(Error::Type type, std::optional<std::string_view> message = std::nullopt);

        void createCall(int functionRegister, uint16_t dstVReg, uint16_t argsVReg);
        void createCall(const x86::Mem& function, uint16_t dstVReg, uint16_t argsVReg);
        void createCall(executor::Function* function, uint16_t dstVReg, uint16_t argsVReg);

        Label createNullCheck(int objectReg);
        Label createObjectKindCheck(int objectReg, oop::ObjectKind expectedKind);
    };
}

#endif // BIBBLEVM_COMPILER_COMPILER_H
