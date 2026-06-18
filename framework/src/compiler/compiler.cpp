// Copyright 2026 Jannik Laugmand Bülow

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
        return nullptr;
    }
}
