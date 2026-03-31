// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_INTERPRETER_H
#define BIBBLEVM_INTERPRETER_H 1

#include "BibbleVM/executor/stack.h"

#include "BibbleVM/api.h"

#include <cstdint>

namespace bibblevm {
    class VM;
}

namespace bibblevm::executor {
    class Function;
    struct InvokeMessage;
    union InstructionArguments;

    enum class InterpreterMessageType {
        NoRequest,
        Errored,
        CallFunction,
        ReturnFromFunction,
        YieldExecution,
        AwaitFuture,
    };

    struct InterpreterMessage {
        InterpreterMessageType type;
        union {
            struct { Function* function; uint16_t argsBegin; } call;
            uint16_t returnRegister;
            Future* future;
        };

        constexpr InterpreterMessage(InterpreterMessageType type) : type(type) {}

        static constexpr InterpreterMessage NoRequest() { return InterpreterMessageType::NoRequest; }
        static constexpr InterpreterMessage Errored() { return InterpreterMessageType::Errored; }
        static constexpr InterpreterMessage CallFunction(Function* function, uint16_t argsBegin) {
            InterpreterMessage m = InterpreterMessageType::CallFunction;
            m.call.function = function;
            m.call.argsBegin = argsBegin;
            return m;
        }
        static constexpr InterpreterMessage ReturnFromFunction(uint16_t returnRegister) {
            InterpreterMessage m = InterpreterMessageType::ReturnFromFunction;
            m.returnRegister = returnRegister;
            return m;
        }
        static constexpr InterpreterMessage YieldExecution() { return InterpreterMessageType::YieldExecution; }
        static constexpr InterpreterMessage AwaitFuture(Future* future) {
            InterpreterMessage m = InterpreterMessageType::AwaitFuture;
            m.future = future;
            return m;
        }
    };

    using Interpreter = InterpreterMessage(*)(VM& vm, Frame& frame, const InstructionArguments& args);

    BIBBLEVM_EXPORT Interpreter GetInterpreter(VM& vm, uint8_t opcode);

    BIBBLEVM_EXPORT InvokeMessage BytecodeInterpreter(VM& vm, Frame& frame);
}

#endif // BIBBLEVM_INTERPRETER_H
