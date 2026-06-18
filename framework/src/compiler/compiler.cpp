// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/compiler/abi.h"
#include "BibbleVM/compiler/compiler.h"

#include <vasm/instruction/singleOperandInstruction/CallInstruction.h>
#include <vasm/instruction/singleOperandInstruction/DeclInstruction.h>
#include <vasm/instruction/singleOperandInstruction/JmpInstruction.h>
#include <vasm/instruction/singleOperandInstruction/JccInstruction.h>
#include <vasm/instruction/singleOperandInstruction/PushInstruction.h>
#include <vasm/instruction/singleOperandInstruction/PopInstruction.h>
#include <vasm/instruction/singleOperandInstruction/IncDecInstruction.h>
#include <vasm/instruction/singleOperandInstruction/IntInstruction.h>
#include <vasm/instruction/singleOperandInstruction/Grp4Instruction.h>
#include <vasm/instruction/singleOperandInstruction/SetccInstruction.h>

#include <vasm/instruction/twoOperandInstruction/MovInstruction.h>
#include <vasm/instruction/twoOperandInstruction/LeaInstruction.h>
#include <vasm/instruction/twoOperandInstruction/ShiftRotateInstruction.h>
#include <vasm/instruction/twoOperandInstruction/XchgInstruction.h>
#include <vasm/instruction/twoOperandInstruction/LogicalInstruction.h>
#include <vasm/instruction/twoOperandInstruction/OutInInstruction.h>
#include <vasm/instruction/twoOperandInstruction/TestInstruction.h>

#include <vasm/instruction/NoOperandInstruction.h>

namespace bibblevm::compiler {
    Code* Compiler::compile(VM& vm, executor::Function* function, CompileOptions options) {
        executor::Instruction* instructionsBegin = function->getInstructions();
        executor::Instruction* instructionsEnd = instructionsBegin + function->getInstructionCount();

        std::vector<instruction::ValuePtr> machineInstructions;

        for (executor::Instruction* inst = instructionsBegin; inst != instructionsEnd; inst++) {
            switch (inst->opcode) {
                case LOAD_IMM: {
                    instruction::RegisterPtr regs = std::make_unique<instruction::Register>(abi::REGS_REGISTER, codegen::OperandSize::Quad);
                    instruction::MemoryPtr dst = std::make_unique<instruction::Memory>(std::move(regs), inst->a * 16 + 8, nullptr, std::nullopt); // TODO: make values 8 bytes
                    instruction::ImmediatePtr imm = std::make_unique<instruction::Immediate>(inst->imm);
                    machineInstructions.push_back(std::make_unique<instruction::MovInstruction>(std::move(dst), std::move(imm)));
                    break;
                }

                default:
                    break;
            }
        }

        for (const instruction::ValuePtr& inst : machineInstructions) {
            inst->print(std::cout);
        }

        return nullptr;
    }
}
