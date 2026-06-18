// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/compiler/abi.h"
#include "BibbleVM/compiler/compiler.h"

#include "BibbleVM/vm.h"

namespace bibblevm::compiler {
    Code* Compiler::compile(VM& vm, executor::Function* function, CompileOptions options) {
        executor::Instruction* instructionsBegin = function->getInstructions();
        executor::Instruction* instructionsEnd = instructionsBegin + function->getInstructionCount();

        for (executor::Instruction* inst = instructionsBegin; inst != instructionsEnd; inst++) {
            switch (inst->opcode) {
                case LOAD_IMM: {
                    break;
                }

                case RETURN: {
                    break;
                }

                default:
                    return nullptr;
            }
        }
    }
}
