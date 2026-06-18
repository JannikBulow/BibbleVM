// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_COMPILER_COMPILER_H
#define BIBBLEVM_COMPILER_COMPILER_H 1

#include "BibbleVM/core/executor/function.h"

#include <vasm/codegen/Opcodes.h>

#include <vasm/instruction/operand/Register.h>

#include <optional>

namespace bibblevm::compiler {
    enum class OptLevel {
        O0,
    };

    struct CompileOptions {
        OptLevel opt;
    };

    class BIBBLEVM_EXPORT Compiler {
    public:
        Code* compile(VM& vm, executor::Function* function, CompileOptions options);

    private:
        instruction::OperandPtr imm(uint64_t value);
        instruction::OperandPtr label(std::string name);
        instruction::OperandPtr memory(instruction::RegisterPtr baseReg, std::optional<int> displacement = std::nullopt, instruction::RegisterPtr indexReg = nullptr, std::optional<int> scale = std::nullopt);
        instruction::OperandPtr reg(int id, codegen::OperandSize size = codegen::OperandSize::Quad);
    };
}

#endif // BIBBLEVM_COMPILER_COMPILER_H
