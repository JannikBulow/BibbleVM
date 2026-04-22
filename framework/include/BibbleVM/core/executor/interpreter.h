// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_CORE_INTERPRETER_H
#define BIBBLEVM_CORE_INTERPRETER_H 1

#include "BibbleVM/core/executor/stack.h"

#include "BibbleVM/core/oop/object.h"

#include "BibbleVM/core/opcodes.h"

#include "BibbleVM/api.h"

#include <cstdint>

namespace bibblevm {
    class VM;
}

namespace bibblevm::executor {
    class Function;
    struct SchedulerMessage;
    struct InstructionArguments;

    enum class InterpreterMessageType {
        Continue,
        Branch,
        Errored,
        CallFunction,
        ReturnFromFunction,
        YieldExecution,
        AwaitFuture,
    };

    struct InterpreterMessage {
        InterpreterMessageType type;
        union {
            int32_t branch;
            struct { Function* function; uint16_t destinationRegister; uint16_t argsBegin; } call;
            uint16_t returnRegister;
            oop::Future* future;
        };

        constexpr InterpreterMessage(InterpreterMessageType type) : type(type) {}

        static constexpr InterpreterMessage Continue() { return InterpreterMessageType::Continue; }
        static constexpr InterpreterMessage Branch(int32_t branch) {
            InterpreterMessage m = InterpreterMessageType::Branch;
            m.branch = branch;
            return m;
        }
        static constexpr InterpreterMessage Errored() { return InterpreterMessageType::Errored; }
        static constexpr InterpreterMessage CallFunction(Function* function, uint16_t destinationRegister, uint16_t argsBegin) {
            InterpreterMessage m = InterpreterMessageType::CallFunction;
            m.call.function = function;
            m.call.destinationRegister = destinationRegister;
            m.call.argsBegin = argsBegin;
            return m;
        }
        static constexpr InterpreterMessage ReturnFromFunction(uint16_t returnRegister) {
            InterpreterMessage m = InterpreterMessageType::ReturnFromFunction;
            m.returnRegister = returnRegister;
            return m;
        }
        static constexpr InterpreterMessage YieldExecution() { return InterpreterMessageType::YieldExecution; }
        static constexpr InterpreterMessage AwaitFuture(oop::Future* future) {
            InterpreterMessage m = InterpreterMessageType::AwaitFuture;
            m.future = future;
            return m;
        }
    };

    using Interpreter = InterpreterMessage(*)(VM& vm, Frame& frame, Task* task, const InstructionArguments& args);

    BIBBLEVM_EXPORT Interpreter GetInterpreter(VM& vm, Opcode opcode);

    BIBBLEVM_EXPORT SchedulerMessage BytecodeInterpreter(VM& vm, Frame& frame, Task* task);
    BIBBLEVM_EXPORT SchedulerMessage AutoYieldingBytecodeInterpreter(VM& vm, Frame& frame, Task* task);
}

#endif // BIBBLEVM_CORE_INTERPRETER_H
