// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_COMPILER_COMPILER_H
#define BIBBLEVM_COMPILER_COMPILER_H 1

#include "BibbleVM/compiler/abi.h"

#include "BibbleVM/core/executor/function.h"

#include "BibbleVM/core/error.h"

#include <asmjit/core.h>
#include <asmjit/x86.h>

#include <functional>
#include <memory>
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
        enum class VRegLocation : uint8_t {
            UntaggedPhysical,
            Physical,
            SpillSlot
        };

        struct VReg {
            VRegLocation location;
            uint16_t id;

            union {
                x86::Gp untaggedPhysical;
                x86::Vec physical;
                x86::Mem address;
            };

            VReg(uint16_t id, const x86::Gp& untaggedPhysical);
            VReg(uint16_t id, const x86::Vec& physical);
            VReg(uint16_t id, const x86::Mem& address);
            VReg(const VReg& other);
            ~VReg();
        };

        template<class R>
        struct TempRegister {
            Compiler& owner;
            R r;

            TempRegister(Compiler& owner, R r) : owner(owner), r(r) {}
            ~TempRegister() { owner.deallocateRegister(r); }
            operator R() const { return r; }
            R* operator->() { return &r; }
        };

        using InstructionCompiler = void(Compiler::*)(VM&, x86::Builder& a, executor::Instruction*);
        std::array<InstructionCompiler, 256> mInstructionCompilers;

        executor::Function* mFunction;
        executor::Instruction* mInstructionsBegin;
        executor::Instruction* mInstructionsEnd;

        x86::Builder* mAsm = nullptr;

        std::vector<int> mAllocatedRegisters;
        std::vector<int> mAllocatedVectorRegisters;
        std::vector<std::unique_ptr<VReg>> mVRegs;

        std::vector<Label> mCheckpoints;
        std::vector<Label> mLabels;

        bool compileInstruction(VM& vm, executor::Instruction* inst);

        x86::Gp allocateRegister(std::vector<int> disallowed = {}); // signature defaults to Gp64 (gpq)
        x86::Vec allocateVectorRegister(std::vector<int> disallowed = {}); // signature defaults to Vec128
        TempRegister<x86::Gp> allocateTempRegister(std::vector<int> disallowed = {});
        TempRegister<x86::Vec> allocateTempVectorRegister(std::vector<int> disallowed = {});
        void deallocateRegister(x86::Gp reg);
        void deallocateRegister(x86::Vec reg);
        void resetRegAlloc();

        VReg* getVReg(uint16_t vregId);

        // change a vregs backing. returns its new backing as a usable operand
        x86::Gp assignPhysRegUntagged(VReg* vreg);
        x86::Vec assignPhysReg(VReg* vreg);
        x86::Mem spillVReg(VReg* vreg);

        void moveVReg(VReg* dst, VReg* src);

        void getIsObject(VReg* vreg, x86::Gp isObjectDst);
        void setIsObject(VReg* vreg, x86::Gp isObject);
        void setIsObject(VReg* vreg, bool isObject);

        void getValue(VReg* vreg, x86::Gp valueDst);
        void setValue(VReg* vreg, x86::Gp value);
        void setValue(VReg* vreg, uint64_t value);

        void compareVReg(VReg* a, VReg* b);
        void compareVReg(VReg* a, uint32_t b);

        x86::Mem getIsObjectAddress(uint16_t index); // deprecated and only used within the vreg operators
        x86::Mem getValueAddress(uint16_t index); // deprecated and only used within the vreg operators
        x86::Mem getFullRegisterAddress(uint16_t index); // deprecated and only used within the vreg operators

        void createArrayLoad(VReg* object, VReg* index, x86::Gp elementSize, x86::Gp dst);
        void createArrayStore(VReg* object, VReg* index, x86::Gp elementSize, x86::Gp src);

        void createPlatformCall(x86::Gp returnRegister, std::function<void()> setupArguments, void* function); // for now, this requires manually moving arguments to the correct registers

        uintptr_t createCheckpoint();
        void bindCheckpoint(uintptr_t checkpoint);
        void createLeave(abi::LeaveReason reason, std::function<void()> populateExitRegisters, bool generateCheckpoint);

        void createError(Error::Type type, std::optional<std::string_view> message = std::nullopt);

        void createCall(const x86::Gp& function, uint16_t dstVReg, uint16_t argsVReg);
        void createCall(const x86::Mem& function, uint16_t dstVReg, uint16_t argsVReg);
        void createCall(executor::Function* function, uint16_t dstVReg, uint16_t argsVReg);

        Label createNullCheck(VReg* object);
        Label createObjectKindCheck(VReg* object, oop::ObjectKind expectedKind);

        void withArrayGuard(VReg* object, std::function<void()> body);



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
