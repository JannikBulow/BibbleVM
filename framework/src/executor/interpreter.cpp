// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/core/opcodes.h"

#include "BibbleVM/executor/function.h"
#include "BibbleVM/executor/instruction.h"
#include "BibbleVM/executor/interpreter.h"

#include "BibbleVM/vm.h"

#define DEFINE_INTERPRETER(opcode) InterpreterMessage Interpret##opcode(VM& vm, Frame& frame, const InstructionArguments& args)
#define REGISTER_INTERPRETER(opcode) table[opcode] = Interpret##opcode

namespace bibblevm::executor {
    DEFINE_INTERPRETER(NOP) {
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(MOV) {
        frame[args.generic.a] = frame[args.generic.b];
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(MOV_HOT_EXT) {
        frame[args.extAB.ab] = frame[args.extAB.c];
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(MOV_EXT_HOT) {
        frame[args.extBC.a] = frame[args.extBC.bc];
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(MOV_RANGE) {
        memmove(&frame[args.generic.a], &frame[args.generic.b], args.generic.c * sizeof(Value));
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(SWAP) {
        Value tmp = frame[args.generic.a];
        frame[args.generic.a] = frame[args.generic.b];
        frame[args.generic.b] = tmp;
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(SWAP_HOT_EXT) {
        Value tmp = frame[args.extBC.a];
        frame[args.extBC.a] = frame[args.extBC.bc];
        frame[args.extBC.bc] = tmp;
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(LOAD_CONST) {
        frame[args.extBC.a] = frame.getFunction().mergedConstPool().get(args.extBC.bc);
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(LOAD_ZERO) {
        frame[args.extAB.ab].ul = 0;
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(LOAD_ONE) {
        frame[args.extAB.ab].ul = 1;
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(LOADB) {
        frame[args.extAB.ab].ul = args.extAB.c;
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(LOADS) {
        frame[args.extBC.a].ul = args.extBC.bc;
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(ADD) {
        frame[args.generic.a].l = frame[args.generic.b].l + frame[args.generic.c].l;
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(SUB) {
        frame[args.generic.a].l = frame[args.generic.b].l - frame[args.generic.c].l;
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(MUL) {
        frame[args.generic.a].l = frame[args.generic.b].l * frame[args.generic.c].l;
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(DIV) {
        frame[args.generic.a].l = frame[args.generic.b].l / frame[args.generic.c].l;
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(MOD) {
        frame[args.generic.a].l = frame[args.generic.b].l % frame[args.generic.c].l;
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(NEG) {
        frame[args.generic.a].l = -frame[args.generic.b].l;
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(CALL) {
        return InterpreterMessage::CallFunction(frame[args.extBC.a].fi, args.extBC.bc);
    }

    DEFINE_INTERPRETER(TAIL_CALL) {
        return InterpreterMessage::Errored(); // TODO: implement
    }

    DEFINE_INTERPRETER(RETURN) {
        return InterpreterMessage::ReturnFromFunction(args.extAB.ab);
    }

    DEFINE_INTERPRETER(ASYNC_CALL) {
        Task* task = vm.scheduler().schedule(*frame[args.extBC.a].fi);
        memcpy(&(*task->stack.getTop())[0], &frame[args.extBC.bc], task->stack.getTop()->getFunction().getParameterCount() * sizeof(Value)); // I'm sorry.
        frame[0].future = task->completionFuture;
        return InterpreterMessage::NoRequest();
    }

    // This interpreter gets called again once the future is completed if it wasn't completed before reaching this.
    // That design is technically a little inefficient, but it's way easier and better looking than the alternatives.
    DEFINE_INTERPRETER(AWAIT) {
        Future* future = frame[args.extBC.a].future;
        if (future->ready) {
            frame[args.extBC.bc] = future->value;
            return InterpreterMessage::NoRequest();
        }
        return InterpreterMessage::AwaitFuture(future);
    }

    DEFINE_INTERPRETER(YIELD) {
        return InterpreterMessageType::YieldExecution;
    }

    Interpreter GetInterpreter(VM& vm, uint8_t opcode) {
        static const std::array<Interpreter, 256> table = [] {
            std::array<Interpreter, 256> table{};
            REGISTER_INTERPRETER(NOP);
            REGISTER_INTERPRETER(MOV);
            REGISTER_INTERPRETER(MOV_HOT_EXT);
            REGISTER_INTERPRETER(MOV_EXT_HOT);
            REGISTER_INTERPRETER(MOV_RANGE);
            REGISTER_INTERPRETER(SWAP);
            REGISTER_INTERPRETER(SWAP_HOT_EXT);
            REGISTER_INTERPRETER(LOAD_CONST);
            REGISTER_INTERPRETER(LOAD_ZERO);
            REGISTER_INTERPRETER(LOAD_ONE);
            REGISTER_INTERPRETER(LOADB);
            REGISTER_INTERPRETER(LOADS);
            REGISTER_INTERPRETER(ADD);
            REGISTER_INTERPRETER(SUB);
            REGISTER_INTERPRETER(MUL);
            REGISTER_INTERPRETER(DIV);
            REGISTER_INTERPRETER(MOD);
            REGISTER_INTERPRETER(NEG);
            REGISTER_INTERPRETER(CALL);
            REGISTER_INTERPRETER(TAIL_CALL);
            REGISTER_INTERPRETER(RETURN);
            REGISTER_INTERPRETER(ASYNC_CALL);
            REGISTER_INTERPRETER(AWAIT);
            REGISTER_INTERPRETER(YIELD);
            return table;
        }();
        return table[opcode];
    }

    SchedulerMessage BytecodeInterpreter(VM& vm, Frame& frame) {
        Instruction*& instruction = frame.ip();
        while (true) {
            InterpreterMessage message = instruction->interpreter(vm, frame, instruction->args);
            switch (message.type) {
                case InterpreterMessageType::NoRequest:
                    instruction++;
                    break;
                case InterpreterMessageType::Errored:
                    return SchedulerMessage::Errored();
                case InterpreterMessageType::CallFunction:
                    instruction++;
                    return SchedulerMessage::Called(message.call.function, message.call.argsBegin);
                case InterpreterMessageType::ReturnFromFunction:
                    return SchedulerMessage::Returned(frame[message.returnRegister]);
                case InterpreterMessageType::YieldExecution:
                    instruction++;
                    return SchedulerMessage::Yielded();
                case InterpreterMessageType::AwaitFuture:
                    return SchedulerMessage::Awaiting(message.future);
            }
        }
    }
}
