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

    using EntryPoint = SchedulerMessage(*)(VM& vm, Frame& frame);

    enum class FunctionKind : uint8_t {
        Normal,
        Native,
    };

    class BIBBLEVM_EXPORT Function {
    public:
        explicit Function(String name); // partial initializing for linker shit
        Function(String name, FunctionKind kind, uint16_t registerCount, uint16_t parameterCount, ConstPool constPool, ConstPool mergedConstPool, Instruction* instructions);

        String getName() const { return mName; }
        FunctionKind getKind() const { return mKind; }
        uint16_t getRegisterCount() const { return mRegisterCount; }
        uint16_t getParameterCount() const { return mParameterCount; }
        const ConstPool& constPool() const { return mConstPool; }
        const ConstPool& mergedConstPool() const { return mMergedConstPool; }
        Instruction* getInstructions() const { return mInstructions; }
        EntryPoint& entryPoint() { return mEntryPoint; }

        SchedulerMessage invoke(VM& vm, Frame& frame) const;

    private:
        String mName;
        FunctionKind mKind;

        uint16_t mRegisterCount;
        uint16_t mParameterCount;

        ConstPool mConstPool; // for viewing the function const pool. never really used for reals
        ConstPool mMergedConstPool;

        Instruction* mInstructions; // No size needed here as the verifier will ensure us that all paths terminate appropriately

        EntryPoint mEntryPoint;
    };
}

#endif //BIBBLEVM_CORE_EXECUTOR_FUNCTION_H