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

#include "vasm/instruction/Label.h"

namespace bibblevm::compiler {
    Code* Compiler::compile(VM& vm, executor::Function* function, CompileOptions options) {
        executor::Instruction* instructionsBegin = function->getInstructions();
        executor::Instruction* instructionsEnd = instructionsBegin + function->getInstructionCount();

        mMachineCode.clear();

        for (executor::Instruction* inst = instructionsBegin; inst != instructionsEnd; inst++) {
            switch (inst->opcode) {
                case LOAD_IMM: {
                    VMRegister dst = vmReg(inst->a);

                    addValue<instruction::MovInstruction>(std::move(dst.isObjectAddress), imm(0), codegen::OperandSize::Byte); //TODO: figure out a single byte write

                    if (inst->imm > 0xFFFFFFFF) {
                        addValue<instruction::MovInstruction>(reg(0), imm(inst->imm));
                        addValue<instruction::MovInstruction>(std::move(dst.valueAddress), reg(0)); //TODO: more generic register stuff instead of hardcoded rax
                    } else {
                        addValue<instruction::MovInstruction>(std::move(dst.valueAddress), imm(inst->imm));
                    }
                    break;
                }

                default:
                    break;
            }
        }

        for (const instruction::ValuePtr& inst : mMachineCode) {
            inst->print(std::cout);
            std::cout << "\n";
        }

        return nullptr;
    }

    instruction::OperandPtr Compiler::imm(uint64_t value) {
        return std::make_unique<instruction::Immediate>(value);
    }

    instruction::OperandPtr Compiler::label(std::string name) {
        return std::make_unique<instruction::LabelOperand>(name);
    }

    instruction::OperandPtr Compiler::memory(instruction::RegisterPtr baseReg, std::optional<int> displacement, instruction::RegisterPtr indexReg, std::optional<int> scale) {
        return std::make_unique<instruction::Memory>(std::move(baseReg), displacement, std::move(indexReg), scale);
    }

    instruction::RegisterPtr Compiler::reg(int id, codegen::OperandSize size) {
        return std::make_unique<instruction::Register>(id, size);
    }

    Compiler::VMRegister Compiler::vmReg(uint16_t index) {
        int displacement = index * sizeof(Value);
        return {memory(reg(abi::REGS_REGISTER), displacement == 0 ? std::nullopt : std::optional(displacement)), memory(reg(abi::REGS_REGISTER), displacement + 8)};
    }
}
