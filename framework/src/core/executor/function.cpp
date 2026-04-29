// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/core/executor/function.h"

#include <iostream>

namespace bibblevm::executor {
    static SchedulerMessage SafetyNet(VM& vm, Frame& frame, Task* task) {
        std::cout << "Interpreter SafetyNet triggered by: " << frame.getFunction().getName() << std::endl;
        return SchedulerMessage::Returned(Value());
    }

    Function::Function(Module& module, String name)
        : mModule(&module)
        , mName(name)
        , mKind(FunctionKind::Normal)
        , mRegisterCount(0)
        , mParameterCount(0)
        , mInstructions(nullptr)
        , mEntryPoint(SafetyNet) {}

    Function::Function(Module& module, String name, FunctionKind kind, uint16_t registerCount, uint16_t parameterCount, Instruction* instructions)
        : mModule(&module)
        , mName(name)
        , mKind(kind)
        , mRegisterCount(registerCount)
        , mParameterCount(parameterCount)
        , mInstructions(instructions)
        , mEntryPoint(SafetyNet) {}

    SchedulerMessage Function::invoke(VM& vm, Frame& frame, Task* task) const {
        return mEntryPoint(vm, frame, task);
    }
}
