// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/executor/function.h"

#include <iostream>

namespace bibblevm::executor {
    static SchedulerMessage SafetyNet(VM& vm, Frame& frame) {
        std::cout << "Interpreter SafetyNet triggered by: " << frame.getFunction().getName() << std::endl;
        return SchedulerMessage::Returned(Value());
    }

    Function::Function(const String& name)
        : mName(name)
        , mKind(FunctionKind::Normal)
        , mRegisterCount(0)
        , mParameterCount(0)
        , mConstPool()
        , mMergedConstPool()
        , mInstructions(nullptr)
        , mEntryPoint(SafetyNet) {}

    Function::Function(const String& name, FunctionKind kind, uint16_t registerCount, uint16_t parameterCount, ConstPool constPool, ConstPool mergedConstPool, Instruction* instructions)
        : mName(name)
        , mKind(kind)
        , mRegisterCount(registerCount)
        , mParameterCount(parameterCount)
        , mConstPool(std::move(constPool))
        , mMergedConstPool(std::move(mergedConstPool))
        , mInstructions(instructions)
        , mEntryPoint(SafetyNet) {}

    SchedulerMessage Function::invoke(VM& vm, Frame& frame) const {
        return mEntryPoint(vm, frame);
    }
}
