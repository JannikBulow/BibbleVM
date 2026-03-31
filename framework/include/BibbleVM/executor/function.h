// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_EXECUTOR_FUNCTION_H
#define BIBBLEVM_EXECUTOR_FUNCTION_H 1

#include "BibbleVM/executor/const_pool.h"
#include "BibbleVM/executor/instruction.h"
#include "BibbleVM/executor/task.h"

#include "BibbleVM/util/string_pool.h"

namespace bibblevm {
    class VM;
}

namespace bibblevm::executor {
    class Function;

    enum class InvokeMessageType {
        Errored,
        Called,
        Returned,
        Yielded,
        Awaiting
    };

    struct InvokeMessage {
        InvokeMessageType type;
        union {
            struct { Function* function; uint16_t argsBegin; } call;
            Value returnValue;
            Future* future;
        };

        constexpr InvokeMessage(InvokeMessageType type) : type(type) {}

        static constexpr InvokeMessage Errored() { return InvokeMessageType::Errored; }
        static constexpr InvokeMessage Called(Function* function, uint16_t argsBegin) {
            InvokeMessage m = InvokeMessageType::Called;
            m.call.function = function;
            m.call.argsBegin = argsBegin;
            return m;
        }
        static constexpr InvokeMessage Returned(Value returnValue) {
            InvokeMessage m = InvokeMessageType::Returned;
            m.returnValue = returnValue;
            return m;
        }
        static constexpr InvokeMessage Yielded() { return InvokeMessageType::Yielded; }
        static constexpr InvokeMessage Awaiting(Future* future) {
            InvokeMessage m = InvokeMessageType::Awaiting;
            m.future = future;
            return m;
        }
    };

    using EntryPoint = InvokeMessage(*)(VM& vm, Frame& frame);

    enum class FunctionKind : uint8_t {
        Normal,
        Native,
    };

    class BIBBLEVM_EXPORT Function {
    public:
        explicit Function(const String& name); // partial initializing for linker shit
        Function(const String& name, FunctionKind kind, uint16_t registerCount, uint16_t parameterCount, ConstPool constPool, ConstPool mergedConstPool, Instruction* instructions);

        String getName() const { return mName; }
        FunctionKind getKind() const { return mKind; }
        uint16_t getRegisterCount() const { return mRegisterCount; }
        uint16_t getParameterCount() const { return mParameterCount; }
        const ConstPool& constPool() const { return mConstPool; }
        const ConstPool& mergedConstPool() const { return mMergedConstPool; }
        Instruction* getInstructions() const { return mInstructions; }
        EntryPoint& entryPoint() { return mEntryPoint; }

        InvokeMessage invoke(VM& vm, Frame& frame) const;

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

#endif //BIBBLEVM_EXECUTOR_FUNCTION_H