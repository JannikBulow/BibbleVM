// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_CORE_EXECUTOR_FUNCTION_H
#define BIBBLEVM_CORE_EXECUTOR_FUNCTION_H 1

#include "BibbleVM/core/executor/const_pool.h"
#include "BibbleVM/core/executor/instruction.h"
#include "BibbleVM/core/executor/scheduler_message.h"
#include "BibbleVM/core/executor/task.h"

#include "BibbleVM/util/string_pool.h"

namespace bibblevm {
    class VM;
}

namespace bibblevm::executor {
    class Function;

    using EntryPoint = SchedulerMessage(*)(VM& vm, Frame& frame, Task* task);

    enum class FunctionKind : uint8_t {
        Normal,
        Native,
    };

    class BIBBLEVM_EXPORT Function {
    public:
        Function(Module& module, String name); // partial initializing for linker shit
        Function(Module& module, String name, FunctionKind kind, uint16_t registerCount, uint16_t parameterCount, Instruction* instructions);

        Module& getModule() const { return *mModule; }
        String getName() const { return mName; }
        FunctionKind getKind() const { return mKind; }
        uint16_t getRegisterCount() const { return mRegisterCount; }
        uint16_t getParameterCount() const { return mParameterCount; }
        Instruction* getInstructions() const { return mInstructions; }
        void*& implementation() { return mImplementation; }
        EntryPoint& entryPoint() { return mEntryPoint; }

        SchedulerMessage invoke(VM& vm, Frame& frame, Task* task) const;

    private:
        Module* mModule;

        String mName;
        FunctionKind mKind;

        uint16_t mRegisterCount;
        uint16_t mParameterCount;

        union {
            Instruction* mInstructions; // No size needed here as the verifier will ensure us that all paths terminate appropriately
            void* mImplementation; // native functions
        };

        EntryPoint mEntryPoint;
    };
}

#endif //BIBBLEVM_CORE_EXECUTOR_FUNCTION_H