// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/compiler/abi.h"
#include "BibbleVM/compiler/compiler.h"

#include "BibbleVM/core/error.h"

#include "BibbleVM/vm.h"

namespace bibblevm::compiler {
    Compiler::Compiler(executor::Function* function)
        : mFunction(function)
        , mInstructionsBegin(function->getInstructions())
        , mInstructionsEnd(function->getInstructions() + function->getInstructionCount()) {}

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

        builder.lea(x86::gpq(abi::GPRS[0]), x86::ptr(checkpointTable));
        builder.jmp(x86::qword_ptr(x86::gpq(abi::GPRS[0]), x86::gpq(abi::RESUME_CHECKPOINT_REGISTER), 3));

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
        auto& a = *mAsm;

        switch (inst->opcode) {
            case NOP:
                a.nop();
                break;

            case MOV: {
                a.mov(x86::gpq(abi::GPRS[0]), getIsObjectAddress(inst->b));
                a.mov(x86::gpq(abi::GPRS[1]), getValueAddress(inst->b));
                a.mov(getIsObjectAddress(inst->a), x86::gpq(abi::GPRS[0]));
                a.mov(getValueAddress(inst->a), x86::gpq(abi::GPRS[1]));
                break;
            }

            case LOAD_IMM: {
                a.mov(getIsObjectAddress(inst->a), 0);
                if (inst->imm > 0xFFFFFFFF) {
                    a.mov(x86::gpq(abi::GPRS[0]), inst->imm);
                    a.mov(getValueAddress(inst->a), x86::gpq(abi::GPRS[0]));
                } else {
                    a.mov(getValueAddress(inst->a), inst->imm);
                }
                break;
            }

            case LOAD_NULL: {
                a.mov(getIsObjectAddress(inst->a), 1);
                a.mov(getValueAddress(inst->a), 0);
                break;
            }

            case ICMP: {
                a.mov(x86::gpq(abi::GPRS[0]), getValueAddress(inst->b));
                a.cmp(x86::gpq(abi::GPRS[0]), getValueAddress(inst->c));

                a.mov(x86::gpd(abi::GPRS[0]), 0);
                a.mov(x86::gpq(abi::GPRS[1]), -1);
                a.mov(x86::gpq(abi::GPRS[2]), 1);

                a.cmovl(x86::gpq(abi::GPRS[0]), x86::gpq(abi::GPRS[1]));
                a.cmovg(x86::gpq(abi::GPRS[0]), x86::gpq(abi::GPRS[2]));

                a.mov(getIsObjectAddress(inst->a), 0);
                a.mov(getValueAddress(inst->a), x86::gpq(abi::GPRS[0]));

                break;
            }

            case INC: {
                a.add(getValueAddress(inst->a), inst->imm);
                break;
            }

            case JMP: {
                a.jmp(mLabels[(inst - mInstructionsBegin) + static_cast<int64_t>(inst->imm)]);
                break;
            }

            case JGE: {
                a.cmp(getValueAddress(inst->a), 0);
                a.jge(mLabels[(inst - mInstructionsBegin) + static_cast<int64_t>(inst->imm)]);
                break;
            }

            case ARRAYLENGTH: {
                auto object = x86::gpq(abi::GPRS[0]);

                a.mov(object, getValueAddress(inst->b));

                Label isNull = createNullCheck(abi::GPRS[0]);
                Label isNotArray = createObjectKindCheck(abi::GPRS[0], oop::ObjectKind::Array);
                Label allGood = a.new_label();

                a.mov(x86::gpq(abi::GPRS[1]), x86::qword_ptr(object, offsetof(oop::Array, length)));
                a.mov(getValueAddress(inst->a), x86::gpq(abi::GPRS[1]));
                a.jmp(allGood);

                a.bind(isNull);
                createError(Error::Type::NULL_REFERENCE);

                a.bind(isNotArray);
                createError(Error::Type::INVALID_OBJECT_KIND);

                a.bind(allGood);

                break;
            }

            case ARRAYGET: {
                auto object = x86::gpq(abi::GPRS[0]);

                a.mov(object, getValueAddress(inst->b));

                Label isNull = createNullCheck(abi::GPRS[0]);
                Label isNotArray = createObjectKindCheck(abi::GPRS[0], oop::ObjectKind::Array);
                Label allGood = a.new_label();

                auto elemSize = x86::gpq(abi::GPRS[1]);
                a.mov(x86::gpd(abi::PLATFORM_ABI_ARGUMENT_REGISTERS[0]), x86::byte_ptr(object, offsetof(oop::Array, baseType)));
                createPlatformCall({abi::GPRS[0]}, abi::GPRS[1], (void*) &oop::GetPrimitiveSizeForType);

                auto index = x86::gpq(abi::GPRS[2]);
                a.mov(index, getValueAddress(inst->c));

                Label size1 = a.new_label();
                Label size2 = a.new_label();
                Label size4 = a.new_label();
                Label size8 = a.new_label();
                Label done = a.new_label();

                a.cmp(elemSize.r32(), 1);
                a.je(size1);

                a.cmp(elemSize.r32(), 2);
                a.je(size2);

                a.cmp(elemSize.r32(), 4);
                a.je(size4);

                a.jmp(size8);

                int valueId = abi::GPRS[3];

                a.bind(size1);
                a.movzx(x86::gpd(valueId), x86::byte_ptr(object, index, 0, (int32_t) offsetof(oop::Array, elementBytes)));
                a.jmp(done);

                a.bind(size2);
                a.movzx(x86::gpd(valueId), x86::word_ptr(object, index, 1, (int32_t) offsetof(oop::Array, elementBytes)));
                a.jmp(done);

                a.bind(size4);
                a.mov(x86::gpd(valueId), x86::dword_ptr(object, index, 2, (int32_t) offsetof(oop::Array, elementBytes)));
                a.jmp(done);

                a.bind(size8);
                a.mov(x86::gpq(valueId), x86::qword_ptr(object, index, 3, (int32_t) offsetof(oop::Array, elementBytes)));
                //a.jmp(done); // no need

                a.bind(done);

                a.cmp(x86::byte_ptr(object, offsetof(oop::Array, baseType)), oop::Type::Reference);
                a.mov(x86::gpd(abi::GPRS[4]), 0);
                a.sete(x86::gpb_lo(abi::GPRS[4]));
                a.mov(getIsObjectAddress(inst->a), x86::gpq(abi::GPRS[4]));

                a.mov(getValueAddress(inst->a), x86::gpq(valueId));

                a.jmp(allGood);

                a.bind(isNull);
                createError(Error::Type::NULL_REFERENCE);

                a.bind(isNotArray);
                createError(Error::Type::INVALID_OBJECT_KIND);

                a.bind(allGood);
                break;
            }

            case CALL: {
                //TODO: create resume logic
                createCall(mFunction->getModule().getConstPool().get(inst->b).fni, inst->a, inst->c);
                break;
            }

            case RETURN: {
                a.mov(x86::gpq(abi::LEAVE_REASON_REGISTER), abi::LeaveReason::Return);
                a.mov(x86::gpq(abi::EXIT1_REGISTER), getIsObjectAddress(inst->a));
                a.mov(x86::gpq(abi::EXIT2_REGISTER), getValueAddress(inst->a));
                a.ret();
                break;
            }

            default:
                mAsm = nullptr;
                return false;
        }

        return true;
    }

    x86::Mem Compiler::getIsObjectAddress(uint16_t vregIndex) {
        return x86::byte_ptr(x86::gpq(abi::REGS_REGISTER), vregIndex * static_cast<uint16_t>(sizeof(Value)));
    }

    x86::Mem Compiler::getValueAddress(uint16_t vregIndex) {
        return x86::qword_ptr(x86::gpq(abi::REGS_REGISTER), vregIndex * static_cast<uint16_t>(sizeof(Value)) + 8);
    }

    void Compiler::createPlatformCall(const std::vector<int>& neededRegisters, int returnRegister, void* function) {
        auto& a = *mAsm;

        std::vector<int> popOrder;

        for (int r : neededRegisters) {
            if (std::ranges::find(abi::PLATFORM_ABI_VOLATILE_REGISTERS, r) != abi::PLATFORM_ABI_VOLATILE_REGISTERS.end()) {
                a.push(x86::gpq(r));
                popOrder.insert(popOrder.begin(), r);
            }
        }

        bool alignFix = false;
        size_t stackOffset = popOrder.size() * 8;
        if (stackOffset % 16 != 0) {
            a.sub(x86::rsp, 8);
            alignFix = true;
        }

        a.mov(x86::gpq(abi::PLATFORM_ABI_RETURN_REGISTER), (uint64_t) function);
        a.call(x86::gpq(abi::PLATFORM_ABI_RETURN_REGISTER));

        if (alignFix) {
            a.add(x86::rsp, 8);
        }

        if (returnRegister != abi::PLATFORM_ABI_RETURN_REGISTER) {
            a.mov(x86::gpq(returnRegister), x86::gpq(abi::PLATFORM_ABI_RETURN_REGISTER));
        }

        for (int r : popOrder) {
            a.pop(x86::gpq(r));
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

    Label Compiler::createNullCheck(int objectReg) {
        auto& a = *mAsm;

        Label label = a.new_label();

        a.test(x86::gpq(objectReg), x86::gpq(objectReg)); // will set ZF to 1 if obj is 0
        a.jz(label);

        return label;
    }

    Label Compiler::createObjectKindCheck(int objectReg, oop::ObjectKind expectedKind) {
        auto& a = *mAsm;

        Label label = a.new_label();

        a.cmp(x86::byte_ptr(x86::gpq(objectReg), offsetof(oop::Object, kind)), expectedKind);
        a.jne(label);

        return label;
    }
}
