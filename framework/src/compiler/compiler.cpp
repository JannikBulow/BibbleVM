// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/compiler/abi.h"
#include "BibbleVM/compiler/compiler.h"

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

        x86::Assembler a(&codeHolder);

        FileLogger logger(stdout);
        a.set_logger(&logger);

        mAsm = &a;

        size_t instructionCount = mInstructionsEnd - mInstructionsBegin;

        mLabels.clear();
        mLabels.reserve(instructionCount);
        for (size_t i = 0; i < instructionCount; ++i) mLabels.push_back(a.new_label());

        for (executor::Instruction* inst = mInstructionsBegin; inst != mInstructionsEnd; inst++) {
            a.bind(mLabels[inst - mInstructionsBegin]);
            if (!compileInstruction(vm, inst)) {
                mAsm = nullptr;
                return nullptr;
            }
        }

        Section* text = codeHolder.text_section();
        Code* code = vm.codeAllocator().allocate(text->buffer().size());
        std::memcpy(code->mc, text->buffer().data(), text->buffer().size());

        vm.codeAllocator().markExecutable(code);

        mAsm = nullptr;

        return code;
    }

    bool Compiler::compileInstruction(VM& vm, executor::Instruction* inst) {
        x86::Assembler& a = *mAsm;

        switch (inst->opcode) {
            case NOP:
                a.nop();
                break;

            case MOV: {

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

            case RETURN: {
                a.mov(x86::gpq(abi::EXIT0_REGISTER), abi::LeaveReason::Return);
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
        return x86::byte_ptr(x86::gpq(abi::REGS_REGISTER), vregIndex * sizeof(Value));
    }

    x86::Mem Compiler::getValueAddress(uint16_t vregIndex) {
        return x86::qword_ptr(x86::gpq(abi::REGS_REGISTER), vregIndex * sizeof(Value) + 8);
    }
}
