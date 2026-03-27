// Copyright 2026 JesusTouchMe

#ifndef BIBBLEVM_EXECUTOR_FUNCTION_H
#define BIBBLEVM_EXECUTOR_FUNCTION_H 1

#include "BibbleVM/executor/instruction.h"

#include "BibbleVM/module/function.h"

namespace bibblevm {
    class VM;
}

namespace bibblevm::executor {
    class Function;

    using EntryPoint = void(*)(VM& vm, Function& function);

    enum class FunctionKind : uint8_t {
        Normal,
        Native,
    };

    class BIBBLEVM_EXPORT Function {
    public:
        Function(const String& name, InstructionList instructions);

        String getName() const { return mName; }
        const InstructionList& getInstructions() const { return mInstructions; }

        void entryPoint(VM& vm);

    private:
        String mName;
        InstructionList mInstructions;

        EntryPoint mEntryPoint;
    };

    using FunctionList = List<Function>;
}

#endif //BIBBLEVM_EXECUTOR_FUNCTION_H