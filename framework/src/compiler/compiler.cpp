// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/compiler/abi.h"
#include "BibbleVM/compiler/compiler.h"

#include "BibbleVM/core/error.h"

#include "BibbleVM/_debug.h"
#include "BibbleVM/vm.h"

namespace bibblevm::compiler {
    Compiler::Compiler(executor::Function* function)
        : mFunction(function)
        , mInstructionsBegin(function->getInstructions())
        , mInstructionsEnd(function->getInstructions() + function->getInstructionCount()) {
#define DECL_COMPILER(inst) mInstructionCompilers[static_cast<size_t>(inst)] = &Compiler::compile##inst;
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
    }

    Code* Compiler::compile(VM& vm, CompileOptions options) {
        CodeHolder codeHolder;
        Environment env = Environment::host();
        codeHolder.init(env);

        x86::Builder builder(&codeHolder);
        BaseNode* startNode = builder.cursor();

        mAsm = &builder;

        size_t instructionCount = mInstructionsEnd - mInstructionsBegin;

        mCheckpoints.clear();

        mLabels.clear();
        mLabels.reserve(instructionCount);
        for (size_t i = 0; i < instructionCount; ++i) mLabels.push_back(builder.new_label());

        uintptr_t entry = createCheckpoint();
        bindCheckpoint(entry);

        for (executor::Instruction* inst = mInstructionsBegin; inst != mInstructionsEnd; inst++) {
            builder.bind(mLabels[inst - mInstructionsBegin]);
            if (!compileInstruction(vm, inst)) {
                mAsm = nullptr;
                return nullptr;
            }
        }

        Label checkpointTable = builder.new_label();
        builder.set_cursor(startNode);

        builder.lea(x86::gpq(abi::GP_REGISTERS[0]), x86::ptr(checkpointTable));
        builder.jmp(x86::qword_ptr(x86::gpq(abi::GP_REGISTERS[0]), x86::gpq(abi::RESUME_CHECKPOINT_REGISTER), 3));

        builder.bind(checkpointTable);
        for (auto& checkpoint : mCheckpoints) {
            builder.embed_label(checkpoint);
        }

        x86::Assembler assembler(&codeHolder);

        builder.finalize();
        builder.serialize_to(&assembler);

        codeHolder.flatten();

        size_t codeSize = codeHolder.code_size();
        Code* code = vm.codeAllocator().allocate(codeSize);

        codeHolder.relocate_to_base(reinterpret_cast<uint64_t>(code->mc));

        Section* text = codeHolder.text_section();
        std::memcpy(code->mc, text->buffer().data(), text->buffer().size());
        vm.codeAllocator().markExecutable(code);

        mAsm = nullptr;

        return code;
    }

    bool Compiler::compileInstruction(VM& vm, executor::Instruction* inst) {
        resetRegAlloc();

        InstructionCompiler fn = mInstructionCompilers[inst->opcode];
        if (!fn) {
            mAsm = nullptr;
            return false;
        }

        (this->*fn)(vm, *mAsm, inst);

        return true;
    }

    x86::Gp Compiler::allocateRegister(std::vector<int> disallowed) {
        disallowed.insert(disallowed.end(), mAllocatedRegisters.begin(), mAllocatedRegisters.end());
        for (int gpr : abi::GP_REGISTERS) {
            if (std::ranges::find(disallowed, gpr) == disallowed.end()) {
                mAllocatedRegisters.push_back(gpr);
                return x86::gpq(gpr);
            }
        }

        BIBBLEVM_ASSERT(false && "TODO: better error handling");
        return {};
    }

    x86::Vec Compiler::allocateVectorRegister(std::vector<int> disallowed) {
        disallowed.insert(disallowed.end(), mAllocatedVectorRegisters.begin(), mAllocatedVectorRegisters.end());
        for (int gpr : abi::GP_REGISTERS) {
            if (std::ranges::find(disallowed, gpr) == disallowed.end()) {
                mAllocatedRegisters.push_back(gpr);
                return x86::xmm(gpr);
            }
        }

        BIBBLEVM_ASSERT(false && "TODO: better error handling");
        return {};
    }

    void Compiler::deallocateRegister(x86::Gp reg) {
        std::erase(mAllocatedRegisters, static_cast<int>(reg.id()));
    }

    void Compiler::deallocateRegister(x86::Vec reg) {
        std::erase(mAllocatedVectorRegisters, static_cast<int>(reg.id()));
    }

    void Compiler::resetRegAlloc() {
        mAllocatedRegisters.clear();
        mAllocatedVectorRegisters.clear();
    }

    x86::Mem Compiler::getIsObjectAddress(uint16_t vreg) {
        return x86::byte_ptr(x86::gpq(abi::REGS_REGISTER), vreg * static_cast<uint16_t>(sizeof(Value)));
    }

    x86::Mem Compiler::getValueAddress(uint16_t vreg) {
        return x86::qword_ptr(x86::gpq(abi::REGS_REGISTER), vreg * static_cast<uint16_t>(sizeof(Value)) + 8);
    }

    Compiler::VMRegisterRef Compiler::getRegisterRef(uint16_t vreg) {
        return {getIsObjectAddress(vreg), getValueAddress(vreg)};
    }

    void Compiler::storeVReg(uint16_t vreg, x86::Gp isObject, x86::Gp value) {
        x86::Builder& a = *mAsm;

        auto reg = getRegisterRef(vreg);
        a.mov(reg.isObjectAddr, isObject);
        a.mov(reg.valueAddr, value);
    }

    void Compiler::storeVReg(uint16_t vreg, bool isObject, x86::Gp value) {
        x86::Builder& a = *mAsm;

        auto reg = getRegisterRef(vreg);
        a.mov(reg.isObjectAddr, isObject);
        a.mov(reg.valueAddr, value);
    }

    void Compiler::storeVReg(uint16_t vreg, x86::Gp isObject, uint64_t value) {
        x86::Builder& a = *mAsm;

        auto reg = getRegisterRef(vreg);
        a.mov(reg.isObjectAddr, isObject);
        if (value > 0xFFFFFFFF) {
            auto tmp = allocateRegister();
            a.mov(tmp, value);
            a.mov(reg.valueAddr, tmp);
            deallocateRegister(tmp);
        } else {
            a.mov(reg.valueAddr, value);
        }
    }

    void Compiler::storeVReg(uint16_t vreg, bool isObject, uint64_t value) {
        x86::Builder& a = *mAsm;

        auto reg = getRegisterRef(vreg);
        a.mov(reg.isObjectAddr, isObject);
        if (value > 0xFFFFFFFF) {
            auto tmp = allocateRegister();
            a.mov(tmp, value);
            a.mov(reg.valueAddr, tmp);
            deallocateRegister(tmp);
        } else {
            a.mov(reg.valueAddr, value);
        }
    }

    void Compiler::createArrayLoad(x86::Gp object, x86::Gp index, x86::Gp elementSize, x86::Gp dst) {
        auto& a = *mAsm;

        Label size1 = a.new_label();
        Label size2 = a.new_label();
        Label size4 = a.new_label();
        Label done = a.new_label();

        a.cmp(elementSize.r32(), 1);
        a.je(size1);

        a.cmp(elementSize.r32(), 2);
        a.je(size2);

        a.cmp(elementSize.r32(), 4);
        a.je(size4);

        // fall-through: size 8
        a.mov(dst.r64(), x86::qword_ptr(object, index, 3, offsetof(oop::Array, elementBytes)));
        a.jmp(done);

        a.bind(size1);
        a.movzx(dst.r32(), x86::byte_ptr(object, index, 0, offsetof(oop::Array, elementBytes)));
        a.jmp(done);

        a.bind(size2);
        a.movzx(dst.r32(), x86::word_ptr(object, index, 1, offsetof(oop::Array, elementBytes)));
        a.jmp(done);

        a.bind(size4);
        a.mov(dst.r32(), x86::dword_ptr(object, index, 2, offsetof(oop::Array, elementBytes)));
        // fall through to done

        a.bind(done);
    }

    void Compiler::createArrayStore(x86::Gp object, x86::Gp index, x86::Gp elementSize, x86::Gp src) {

    }

    void Compiler::createPlatformCall(const std::vector<x86::Gp>& neededRegisters, x86::Gp returnRegister, void* function) {
        auto& a = *mAsm;

        std::vector<x86::Gp> popOrder;

        for (auto& r : neededRegisters) {
            if (std::ranges::find(abi::PLATFORM_ABI_VOLATILE_REGISTERS, r.id()) != abi::PLATFORM_ABI_VOLATILE_REGISTERS.end()) {
                a.push(r);
                popOrder.insert(popOrder.begin(), r);
            }
        }

        size_t stackAdjust = 0;
        size_t stackOffset = popOrder.size() * 8 + 8; // +8 represents the return address already on the stack
        if (stackOffset % 16 != 0) {
            stackAdjust += 8;
        }

#if defined(BIBBLEVM_ABI_X64_WIN64)
        stackAdjust += 32;
#endif

        if (stackAdjust != 0) {
            a.sub(x86::rsp, stackAdjust);
        }

        a.mov(x86::gpq(abi::PLATFORM_ABI_RETURN_REGISTER), (uint64_t) function);
        a.call(x86::gpq(abi::PLATFORM_ABI_RETURN_REGISTER));

        if (stackAdjust != 0) {
            a.add(x86::rsp, stackAdjust);
        }

        if (returnRegister.id() != abi::PLATFORM_ABI_RETURN_REGISTER) {
            a.mov(returnRegister, x86::gpq(abi::PLATFORM_ABI_RETURN_REGISTER));
        }

        for (auto& r : popOrder) {
            a.pop(r);
        }
    }

    uintptr_t Compiler::createCheckpoint() {
        auto& a = *mAsm;

        uintptr_t id = mCheckpoints.size();

        Label checkpoint = a.new_label();
        mCheckpoints.push_back(checkpoint);

        return id;
    }

    void Compiler::bindCheckpoint(uintptr_t checkpoint) {
        mAsm->bind(mCheckpoints[checkpoint]);
    }

    void Compiler::createLeave(abi::LeaveReason reason, bool generateCheckpoint) {
        auto& a = *mAsm;

        a.mov(x86::gpq(abi::LEAVE_REASON_REGISTER), reason);

        uintptr_t checkpoint;

        if (generateCheckpoint) {
            checkpoint = createCheckpoint();
            a.mov(x86::gpq(abi::LEAVE_CHECKPOINT_REGISTER), checkpoint);
        }

        a.ret();

        if (generateCheckpoint) {
            bindCheckpoint(checkpoint);
        }
    }

    void Compiler::createError(Error::Type type, std::optional<std::string_view> message) {
        auto& a = *mAsm;

        a.mov(x86::gpq(abi::EXIT1_REGISTER), type);

        if (message.has_value()) {

        } else {
            a.xor_(x86::gpq(abi::EXIT2_REGISTER), x86::gpq(abi::EXIT2_REGISTER));
        }

        createLeave(abi::LeaveReason::Error, false);
    }

    void Compiler::createCall(int functionRegister, uint16_t dstVReg, uint16_t argsVReg) {
        auto& a = *mAsm;

        if (functionRegister != abi::EXIT1_REGISTER) {
            a.mov(x86::gpq(abi::EXIT1_REGISTER), x86::gpq(functionRegister));
        }

        a.mov(x86::gpq(abi::EXIT2_REGISTER), (static_cast<uint32_t>(dstVReg) << 16) | argsVReg);
        createLeave(abi::LeaveReason::Call, true);
    }

    void Compiler::createCall(const x86::Mem& function, uint16_t dstVReg, uint16_t argsVReg) {
        auto& a = *mAsm;

        a.mov(x86::gpq(abi::EXIT1_REGISTER), function);
        a.mov(x86::gpq(abi::EXIT2_REGISTER), (static_cast<uint32_t>(dstVReg) << 16) | argsVReg);
        createLeave(abi::LeaveReason::Call, true);
    }

    void Compiler::createCall(executor::Function* function, uint16_t dstVReg, uint16_t argsVReg) {
        auto& a = *mAsm;

        a.mov(x86::gpq(abi::EXIT1_REGISTER), function);
        a.mov(x86::gpq(abi::EXIT2_REGISTER), (static_cast<uint32_t>(dstVReg) << 16) | argsVReg);
        createLeave(abi::LeaveReason::Call, true);
    }

    Label Compiler::createNullCheck(x86::Gp objectReg) {
        auto& a = *mAsm;

        Label label = a.new_label();

        a.test(objectReg, objectReg); // will set ZF to 1 if obj is 0
        a.jz(label);

        return label;
    }

    Label Compiler::createObjectKindCheck(x86::Gp objectReg, oop::ObjectKind expectedKind) {
        auto& a = *mAsm;

        Label label = a.new_label();

        a.cmp(x86::byte_ptr(objectReg, offsetof(oop::Object, kind)), expectedKind);
        a.jne(label);

        return label;
    }

    void Compiler::withArrayGuard(x86::Gp objectReg, std::function<void()> body) {
        auto& a = *mAsm;

        Label isNull = createNullCheck(objectReg);
        Label isNotArray = createObjectKindCheck(objectReg, oop::ObjectKind::Array);
        Label allGood = a.new_label();

        body();
        a.jmp(allGood);

        a.bind(isNull);
        createError(Error::Type::NULL_REFERENCE);

        a.bind(isNotArray);
        createError(Error::Type::INVALID_OBJECT_KIND);

        a.bind(allGood);
    }


    void Compiler::compileNOP(VM& vm, x86::Builder& a, executor::Instruction* inst) {
        a.nop();
    }

    void Compiler::compileMOV(VM& vm, x86::Builder& a, executor::Instruction* inst) {
        auto tmp0 = allocateRegister();
        auto tmp1 = allocateRegister();
        auto src = getRegisterRef(inst->b);

        a.mov(tmp0, src.isObjectAddr);
        a.mov(tmp1, src.valueAddr);
        storeVReg(inst->a, tmp0, tmp1);
    }

    void Compiler::compileMOV_RANGE(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileSWAP(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileLOAD_CONST(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileLOAD_IMM(VM& vm, x86::Builder& a, executor::Instruction* inst) {
        storeVReg(inst->a, false, inst->imm);
    }

    void Compiler::compileLOAD_NULL(VM& vm, x86::Builder& a, executor::Instruction* inst) {
        storeVReg(inst->a, true, 0);
    }

    void Compiler::compileADD(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileSUB(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileMUL(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileSDIV(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileUDIV(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileSMOD(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileUMOD(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileNEG(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileABS(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileAND(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileOR(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileXOR(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileNOT(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileSHL(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileSHR(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileSAR(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileFADD(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileFSUB(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileFMUL(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileFDIV(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileFNEG(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileFABS(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileTR8(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileTR16(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileTR32(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileSEX8(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileSEX16(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileSEX32(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileZEX8(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileZEX16(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileZEX32(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileI2F(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileU2F(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileI2D(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileU2D(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileF2I(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileF2U(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileD2I(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileD2U(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileF2D(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileD2F(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileICMP(VM& vm, x86::Builder& a, executor::Instruction* inst) {
        auto compareTmp = allocateRegister();

        a.mov(compareTmp, getValueAddress(inst->b));
        a.cmp(compareTmp, getValueAddress(inst->c));

        deallocateRegister(compareTmp);

        auto result = allocateRegister();
        auto lessThan = allocateRegister();
        auto greaterThan = allocateRegister();

        a.mov(result, 0);
        a.mov(lessThan, -1);
        a.mov(greaterThan, 1);

        a.cmovl(result, lessThan);
        a.cmovg(result, greaterThan);

        a.mov(getIsObjectAddress(inst->a), 0);
        a.mov(getValueAddress(inst->a), result);
    }

    void Compiler::compileUCMP(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileFCMP(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileSTRCMP(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileINC(VM& vm, x86::Builder& a, executor::Instruction* inst) {
        a.add(getValueAddress(inst->a), inst->imm);
    }

    void Compiler::compileDEC(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileJMP(VM& vm, x86::Builder& a, executor::Instruction* inst) {
        a.jmp(mLabels[(inst - mInstructionsBegin) + static_cast<int64_t>(inst->imm)]);
    }

    void Compiler::compileJEQ(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileJNE(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileJLT(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileJLE(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileJGT(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileJGE(VM& vm, x86::Builder& a, executor::Instruction* inst) {
        a.cmp(getValueAddress(inst->a), 0);
        a.jge(mLabels[(inst - mInstructionsBegin) + static_cast<int64_t>(inst->imm)]);
    }

    void Compiler::compileNEWINSTANCE(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileNEWARRAY(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileNEWSTRING(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileNEWFUTURE(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileOBJKIND(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileISKIND(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileINSTANCEOF(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileGETFIELD(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileSETFIELD(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileDISPATCHMETHOD(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileGETCLASS(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileARRAYLENGTH(VM& vm, x86::Builder& a, executor::Instruction* inst) {
        auto object = allocateRegister();
        a.mov(object, getValueAddress(inst->b));

        withArrayGuard(object, [&] {
            auto len = allocateRegister();
            a.mov(len, x86::qword_ptr(object, offsetof(oop::Array, length)));
            a.mov(getValueAddress(inst->a), len);
        });
    }

    void Compiler::compileARRAYGET(VM& vm, x86::Builder& a, executor::Instruction* inst) {
        auto object = allocateRegister();
        a.mov(object, getValueAddress(inst->b));

        withArrayGuard(object, [&] {
            auto elemSize = allocateRegister();
            a.mov(x86::gpd(abi::PLATFORM_ABI_ARGUMENT_REGISTERS[0]), x86::byte_ptr(object, offsetof(oop::Array, baseType)));
            createPlatformCall({object}, elemSize, (void*) &oop::GetPrimitiveSizeForType);

            auto index = allocateRegister();
            a.mov(index, getValueAddress(inst->c));

            auto value = allocateRegister();
            createArrayLoad(object, index, elemSize, value);

            deallocateRegister(elemSize);
            deallocateRegister(index);

            auto isObjectFlag = allocateRegister();
            a.xor_(isObjectFlag.r32(), isObjectFlag.r32());
            a.cmp(x86::byte_ptr(object, offsetof(oop::Array, baseType)), oop::Type::Reference);
            a.sete(isObjectFlag.r8_lo());

            storeVReg(inst->a, isObjectFlag, value);
        });
    }

    void Compiler::compileARRAYSET(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileSTRLENGTH(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileSTRGET(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileSTR2ARRAY(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileRESOLVE(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileCANCEL(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileISFUTUREREADY(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compilePOLL(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileAWAIT(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileCALL(VM& vm, x86::Builder& a, executor::Instruction* inst) {
        createCall(mFunction->getModule().getConstPool().get(inst->b).fni, inst->a, inst->c);
    }

    void Compiler::compileTAIL_CALL(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileCALLA(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileCALLAP(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileCALLARP(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileCALL_DYN(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileTAIL_CALL_DYN(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileCALLA_DYN(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileCALLAP_DYN(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileCALLARP_DYN(VM& vm, x86::Builder& a, executor::Instruction* inst) {}

    void Compiler::compileRETURN(VM& vm, x86::Builder& a, executor::Instruction* inst) {
        a.mov(x86::gpq(abi::EXIT1_REGISTER), getIsObjectAddress(inst->a));
        a.mov(x86::gpq(abi::EXIT2_REGISTER), getValueAddress(inst->a));

        createLeave(abi::LeaveReason::Return, false);
    }

    void Compiler::compileYIELD(VM& vm, x86::Builder& a, executor::Instruction* inst) {
        createLeave(abi::LeaveReason::Yield, true);
    }
}
