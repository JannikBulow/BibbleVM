// Copyright 2026 JesusTouchMe

#include "BibbleVM/executor/function.h"

namespace bibblevm::executor {
    static void InterpreterEntryPoint(VM& vm, Function& function) {
        for (const auto& insn : function.getInstructions()) {
            insn.interpreter(vm, insn.a, insn.b, insn.c);
        }
    }

    Function::Function(const String& name, InstructionList instructions)
        : mName(name)
        , mInstructions(std::move(instructions))
        , mEntryPoint(InterpreterEntryPoint) {}

    void Function::entryPoint(VM& vm) {
        mEntryPoint(vm, *this);
    }
}