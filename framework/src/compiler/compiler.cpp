// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/compiler/abi.h"
#include "BibbleVM/compiler/compiler.h"

#include "BibbleVM/vm.h"

namespace bibblevm::compiler {
    Code* Compiler::compile(VM& vm, executor::Function* function, CompileOptions options) {
        CodeHolder codeHolder;
        Environment env = Environment::host();
        codeHolder.init(env);

        x86::Assembler a(&codeHolder);

        FileLogger logger(stdout);
        a.set_logger(&logger);

        executor::Instruction* instructionsBegin = function->getInstructions();
        executor::Instruction* instructionsEnd = instructionsBegin + function->getInstructionCount();

        for (executor::Instruction* inst = instructionsBegin; inst != instructionsEnd; inst++) {
            switch (inst->opcode) {
                case LOAD_IMM: {
                    a.mov(getIsObjectAddress(inst->a), 0);
                    if (inst->imm > 0xFFFFFFFF) {
                        a.mov(x86::gpq(abi::GENERAL_PURPOSE_REGISTERS[0]), inst->imm);
                        a.mov(getValueAddress(inst->a), x86::gpq(abi::GENERAL_PURPOSE_REGISTERS[0]));
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
                    return nullptr;
            }
        }

        Section* text = codeHolder.text_section();
        Code* code = vm.codeAllocator().allocate(text->buffer().size());
        std::memcpy(code->mc, text->buffer().data(), text->buffer().size());

        vm.codeAllocator().markExecutable(code);

        return code;
    }

    x86::Mem Compiler::getIsObjectAddress(uint16_t vregIndex) {
        return x86::byte_ptr(x86::gpq(abi::REGS_REGISTER), vregIndex * sizeof(Value));
    }

    x86::Mem Compiler::getValueAddress(uint16_t vregIndex) {
        return x86::qword_ptr(x86::gpq(abi::REGS_REGISTER), vregIndex * sizeof(Value) + 8);
    }
}
