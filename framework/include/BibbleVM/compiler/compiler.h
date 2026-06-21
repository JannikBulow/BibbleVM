// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_COMPILER_COMPILER_H
#define BIBBLEVM_COMPILER_COMPILER_H 1

#include "BibbleVM/compiler/abi.h"

#include "BibbleVM/core/executor/function.h"

#include "BibbleVM/core/error.h"

#include <asmjit/asmjit.h>

#include <functional>
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
        struct VMRegisterRef {
            x86::Mem isObjectAddr;
            x86::Mem valueAddr;
        };

        using InstructionCompiler = void(Compiler::*)(VM&, x86::Builder& a, executor::Instruction*);
        std::array<InstructionCompiler, 256> mInstructionCompilers;

        executor::Function* mFunction;
        executor::Instruction* mInstructionsBegin;
        executor::Instruction* mInstructionsEnd;

        x86::Builder* mAsm = nullptr;

        std::vector<int> mAllocatedRegisters;
        std::vector<int> mAllocatedVectorRegisters;

        std::vector<Label> mCheckpoints;
        std::vector<Label> mLabels;

        bool compileInstruction(VM& vm, executor::Instruction* inst);

        x86::Gp allocateRegister(std::vector<int> disallowed = {}); // signature defaults to Gp64 (gpq)
        x86::Vec allocateVectorRegister(std::vector<int> disallowed = {}); // signature defaults to Vec128
        void deallocateRegister(x86::Gp reg);
        void deallocateRegister(x86::Vec reg);
        void resetRegAlloc();

        x86::Mem getIsObjectAddress(uint16_t vreg);
        x86::Mem getValueAddress(uint16_t vreg);
        VMRegisterRef getRegisterRef(uint16_t vreg);

        void storeVReg(uint16_t vreg, x86::Gp isObject, x86::Gp value);
        void storeVReg(uint16_t vreg, bool isObject, x86::Gp value);
        void storeVReg(uint16_t vreg, x86::Gp isObject, uint64_t value);
        void storeVReg(uint16_t vreg, bool isObject, uint64_t value);

        void createArrayLoad(x86::Gp object, x86::Gp index, x86::Gp elementSize, x86::Gp dst);
        void createArrayStore(x86::Gp object, x86::Gp index, x86::Gp elementSize, x86::Gp src);

        void createPlatformCall(const std::vector<x86::Gp>& neededRegisters, x86::Gp returnRegister, void* function); // for now, this required manually moving arguments to the correct registers

        uintptr_t createCheckpoint();
        void bindCheckpoint(uintptr_t checkpoint);
        void createLeave(abi::LeaveReason reason, bool generateCheckpoint); // assumes exit1 and exit2 are populated before call

        void createError(Error::Type type, std::optional<std::string_view> message = std::nullopt);

        void createCall(int functionRegister, uint16_t dstVReg, uint16_t argsVReg);
        void createCall(const x86::Mem& function, uint16_t dstVReg, uint16_t argsVReg);
        void createCall(executor::Function* function, uint16_t dstVReg, uint16_t argsVReg);

        Label createNullCheck(x86::Gp objectReg);
        Label createObjectKindCheck(x86::Gp, oop::ObjectKind expectedKind);

        void withArrayGuard(x86::Gp objectReg, std::function<void()> body);



#define DECL_COMPILER(_inst) void compile##_inst(VM& vm, x86::Builder& a, executor::Instruction* inst);
        DECL_COMPILER(NOP);
        DECL_COMPILER(MOV);
        DECL_COMPILER(MOV_RANGE);
        DECL_COMPILER(SWAP);
        DECL_COMPILER(LOAD_CONST);
        DECL_COMPILER(LOAD_IMM);
        DECL_COMPILER(LOAD_NULL);
        DECL_COMPILER(ADD);
        DECL_COMPILER(SUB);
        DECL_COMPILER(MUL);
        DECL_COMPILER(SDIV);
        DECL_COMPILER(UDIV);
        DECL_COMPILER(SMOD);
        DECL_COMPILER(UMOD);
        DECL_COMPILER(NEG);
        DECL_COMPILER(ABS);
        DECL_COMPILER(AND);
        DECL_COMPILER(OR);
        DECL_COMPILER(XOR);
        DECL_COMPILER(NOT);
        DECL_COMPILER(SHL);
        DECL_COMPILER(SHR);
        DECL_COMPILER(SAR);
        DECL_COMPILER(FADD);
        DECL_COMPILER(FSUB);
        DECL_COMPILER(FMUL);
        DECL_COMPILER(FDIV);
        DECL_COMPILER(FNEG);
        DECL_COMPILER(FABS);
        DECL_COMPILER(TR8);
        DECL_COMPILER(TR16);
        DECL_COMPILER(TR32);
        DECL_COMPILER(SEX8);
        DECL_COMPILER(SEX16);
        DECL_COMPILER(SEX32);
        DECL_COMPILER(ZEX8);
        DECL_COMPILER(ZEX16);
        DECL_COMPILER(ZEX32);
        DECL_COMPILER(I2F);
        DECL_COMPILER(U2F);
        DECL_COMPILER(I2D);
        DECL_COMPILER(U2D);
        DECL_COMPILER(F2I);
        DECL_COMPILER(F2U);
        DECL_COMPILER(D2I);
        DECL_COMPILER(D2U);
        DECL_COMPILER(F2D);
        DECL_COMPILER(D2F);
        DECL_COMPILER(ICMP);
        DECL_COMPILER(UCMP);
        DECL_COMPILER(FCMP);
        DECL_COMPILER(STRCMP);
        DECL_COMPILER(INC);
        DECL_COMPILER(DEC);
        DECL_COMPILER(JMP);
        DECL_COMPILER(JEQ);
        DECL_COMPILER(JNE);
        DECL_COMPILER(JLT);
        DECL_COMPILER(JLE);
        DECL_COMPILER(JGT);
        DECL_COMPILER(JGE);
        DECL_COMPILER(NEWINSTANCE);
        DECL_COMPILER(NEWARRAY);
        DECL_COMPILER(NEWSTRING);
        DECL_COMPILER(NEWFUTURE);
        DECL_COMPILER(OBJKIND);
        DECL_COMPILER(ISKIND);
        DECL_COMPILER(INSTANCEOF);
        DECL_COMPILER(GETFIELD);
        DECL_COMPILER(SETFIELD);
        DECL_COMPILER(DISPATCHMETHOD);
        DECL_COMPILER(GETCLASS);
        DECL_COMPILER(ARRAYLENGTH);
        DECL_COMPILER(ARRAYGET);
        DECL_COMPILER(ARRAYSET);
        DECL_COMPILER(STRLENGTH);
        DECL_COMPILER(STRGET);
        DECL_COMPILER(STR2ARRAY);
        DECL_COMPILER(RESOLVE);
        DECL_COMPILER(CANCEL);
        DECL_COMPILER(ISFUTUREREADY);
        DECL_COMPILER(POLL);
        DECL_COMPILER(AWAIT);
        DECL_COMPILER(CALL);
        DECL_COMPILER(TAIL_CALL);
        DECL_COMPILER(CALLA);
        DECL_COMPILER(CALLAP);
        DECL_COMPILER(CALLARP);
        DECL_COMPILER(CALL_DYN);
        DECL_COMPILER(TAIL_CALL_DYN);
        DECL_COMPILER(CALLA_DYN);
        DECL_COMPILER(CALLAP_DYN);
        DECL_COMPILER(CALLARP_DYN);
        DECL_COMPILER(RETURN);
        DECL_COMPILER(YIELD);
#undef DECL_COMPILER
    };
}

#endif // BIBBLEVM_COMPILER_COMPILER_H
