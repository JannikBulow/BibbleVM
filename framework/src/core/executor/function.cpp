// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/core/executor/function.h"

#include "BibbleVM/vm.h"

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
        , mInstructionCount(0)
        , mInstructions(nullptr)
        , mEntryPoint(SafetyNet) {}

    Function::Function(Module& module, String name, FunctionKind kind, uint16_t registerCount, uint16_t parameterCount, Instruction* instructions, uint32_t instructionCount)
        : mModule(&module)
        , mName(name)
        , mKind(kind)
        , mRegisterCount(registerCount)
        , mParameterCount(parameterCount)
        , mInstructionCount(instructionCount)
        , mInstructions(instructions)
        , mEntryPoint(SafetyNet) {}

    Function& Function::operator=(const Function& other) {
        mModule = other.mModule;
        mName = other.mName;
        mKind = other.mKind;
        mRegisterCount = other.mRegisterCount;
        mParameterCount = other.mParameterCount;
        mInvocationCount.store(other.mInvocationCount.load(std::memory_order_relaxed), std::memory_order_relaxed);
        mInstructionCount = other.mInstructionCount;
        mImplementation = other.mImplementation;
        mEntryPoint.store(other.mEntryPoint.load(std::memory_order_relaxed), std::memory_order_relaxed);
        return *this;
    }

    SchedulerMessage Function::invoke(VM& vm, Frame& frame, Task* task) {
        if (++mInvocationCount == vm.config().compiler.jit.hotThreshold && mJitContext.load(std::memory_order_acquire) == nullptr) {
            vm.enqueueForJitCompile(this);
        }

        return mEntryPoint.load(std::memory_order_acquire)(vm, frame, task);
    }
}
