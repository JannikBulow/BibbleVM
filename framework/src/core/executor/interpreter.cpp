// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/core/opcodes.h"

#include "BibbleVM/core/executor/function.h"
#include "BibbleVM/core/executor/instruction.h"
#include "BibbleVM/core/executor/interpreter.h"

#include "BibbleVM/util/safe_math.h"

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

    DEFINE_INTERPRETER(SDIV) {
        frame[args.generic.a].l = frame[args.generic.b].l / frame[args.generic.c].l;
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(UDIV) {
        frame[args.generic.a].ul = frame[args.generic.b].ul / frame[args.generic.c].ul;
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(SMOD) {
        frame[args.generic.a].l = frame[args.generic.b].l % frame[args.generic.c].l;
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(UMOD) {
        frame[args.generic.a].ul = frame[args.generic.b].ul % frame[args.generic.c].ul;
    }

    DEFINE_INTERPRETER(NEG) {
        frame[args.generic.a].l = -frame[args.generic.b].l;
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(ABS) {
        frame[args.generic.a].l = std::abs(frame[args.generic.b].l);
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(AND) {
        frame[args.generic.a].ul = frame[args.generic.b].ul & frame[args.generic.c].ul;
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(OR) {
        frame[args.generic.a].ul = frame[args.generic.b].ul | frame[args.generic.c].ul;
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(XOR) {
        frame[args.generic.a].ul = frame[args.generic.b].ul ^ frame[args.generic.c].ul;
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(NOT) {
        frame[args.generic.a].ul = ~frame[args.generic.b].ul;
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(SHL) {
        frame[args.generic.a].ul = math::LogicalShiftLeft(frame[args.generic.b].ul, frame[args.generic.c].ui);
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(SHR) {
        frame[args.generic.a].ul = math::LogicalShiftRight(frame[args.generic.b].ul, frame[args.generic.c].ui);
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(SAR) {
        frame[args.generic.a].l = math::ArithmeticShiftRight(frame[args.generic.b].l, frame[args.generic.c].ui);
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(FADD) {
        frame[args.generic.a].f = frame[args.generic.b].f + frame[args.generic.c].f;
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(FSUB) {
        frame[args.generic.a].f = frame[args.generic.b].f - frame[args.generic.c].f;
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(FMUL) {
        frame[args.generic.a].f = frame[args.generic.b].f * frame[args.generic.c].f;
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(FDIV) {
        frame[args.generic.a].f = frame[args.generic.b].f / frame[args.generic.c].f;
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(FNEG) {
        frame[args.generic.a].f = -frame[args.generic.b].f;
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(FABS) {
        frame[args.generic.a].f = std::abs(frame[args.generic.b].f);
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(DADD) {
        frame[args.generic.a].d = frame[args.generic.b].d + frame[args.generic.c].d;
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(DSUB) {
        frame[args.generic.a].d = frame[args.generic.b].d - frame[args.generic.c].d;
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(DMUL) {
        frame[args.generic.a].d = frame[args.generic.b].d * frame[args.generic.c].d;
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(DDIV) {
        frame[args.generic.a].d = frame[args.generic.b].d / frame[args.generic.c].d;
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(DNEG) {
        frame[args.generic.a].d = -frame[args.generic.b].d;
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(DABS) {
        frame[args.generic.a].d = std::abs(frame[args.generic.b].d);
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(TR8) {
        frame[args.generic.a].ub = static_cast<UByte>(frame[args.generic.b].ul);
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(TR8S) {
        frame[args.generic.a].b = static_cast<Byte>(frame[args.generic.b].l);
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(TR16) {
        frame[args.generic.a].us = static_cast<UShort>(frame[args.generic.b].ul);
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(TR16S) {
        frame[args.generic.a].s = static_cast<Short>(frame[args.generic.b].l);
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(TR32) {
        frame[args.generic.a].ui = static_cast<UInt>(frame[args.generic.b].ul);
    }

    DEFINE_INTERPRETER(TR32S) {
        frame[args.generic.a].i = static_cast<Int>(frame[args.generic.b].l);
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(SEX8) {
        frame[args.generic.a].l = static_cast<Long>(frame[args.generic.b].b);
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(SEX16) {
        frame[args.generic.a].l = static_cast<Long>(frame[args.generic.b].s);
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(SEX32) {
        frame[args.generic.a].l = static_cast<Long>(frame[args.generic.b].i);
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(ZEX8) {
        frame[args.generic.a].ul = static_cast<ULong>(frame[args.generic.b].ub);
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(ZEX16) {
        frame[args.generic.a].ul = static_cast<ULong>(frame[args.generic.b].us);
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(ZEX32) {
        frame[args.generic.a].ul = static_cast<ULong>(frame[args.generic.b].ui);
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(I2F) {
        frame[args.generic.a].f = static_cast<float>(frame[args.generic.b].l);
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(I2D) {
        frame[args.generic.a].d = static_cast<double>(frame[args.generic.b].l);
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(F2I) {
        frame[args.generic.a].l = static_cast<Long>(frame[args.generic.b].f);
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(D2I) {
        frame[args.generic.a].l = static_cast<Long>(frame[args.generic.b].d);
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(F2D) {
        frame[args.generic.a].d = static_cast<double>(frame[args.generic.b].f);
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(D2F) {
        frame[args.generic.a].f = static_cast<float>(frame[args.generic.b].d);
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(ICMP) {
        Byte& dst = frame[args.generic.a].b;
        Long lhs = frame[args.generic.b].l;
        Long rhs = frame[args.generic.c].l;

        if (lhs < rhs) dst = -1;
        if (lhs > rhs) dst = 1;
        else dst = 0;

        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(UCMP) {
        Byte& dst = frame[args.generic.a].b;
        ULong lhs = frame[args.generic.b].ul;
        ULong rhs = frame[args.generic.c].ul;

        if (lhs < rhs) dst = -1;
        if (lhs > rhs) dst = 1;
        else dst = 0;

        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(FCMP) {
        Byte& dst = frame[args.generic.a].b;
        float lhs = frame[args.generic.b].f;
        float rhs = frame[args.generic.c].f;

        if (lhs < rhs) dst = -1;
        if (lhs > rhs) dst = 1;
        else dst = 0;

        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(DCMP) {
        Byte& dst = frame[args.generic.a].b;
        double lhs = frame[args.generic.b].d;
        double rhs = frame[args.generic.c].d;

        if (lhs < rhs) dst = -1;
        if (lhs > rhs) dst = 1;
        else dst = 0;

        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(STRCMP) {
        String str1 = frame[args.generic.b].obj->asString();
        String str2 = frame[args.generic.c].obj->asString();
        frame[args.generic.a].b = static_cast<Byte>(str1.compareCorrect(str2));
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(INC) {
        frame[args.extBC.a].l += args.extBC.bc;
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(DEC) {
        frame[args.extBC.a].l -= args.extBC.bc;
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(JMP) {
        frame.ip() += args.extJump.branch;
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(JEQ) {
        if (frame[args.extJump.cond].b == 0) frame.ip() += args.extJump.branch;
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(JNE) {
        if (frame[args.extJump.cond].b != 0) frame.ip() += args.extJump.branch;
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(JLT) {
        if (frame[args.extJump.cond].b < 0) frame.ip() += args.extJump.branch;
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(JLE) {
        if (frame[args.extJump.cond].b <= 0) frame.ip() += args.extJump.branch;
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(JGT) {
        if (frame[args.extJump.cond].b > 0) frame.ip() += args.extJump.branch;
        return InterpreterMessage::NoRequest();
    }

    DEFINE_INTERPRETER(JGE) {
        if (frame[args.extJump.cond].b >= 0) frame.ip() += args.extJump.branch;
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
        Task* task = vm.scheduler().schedule(vm, *frame[args.extBC.a].fi, &frame[args.extBC.bc]);
        frame[0].obj = task->completionFuture->asObject();
        return InterpreterMessage::NoRequest();
    }

    // This interpreter gets called again once the future is completed if it wasn't completed before reaching this.
    // That design is technically a little inefficient, but it's way easier and better looking than the alternatives.
    DEFINE_INTERPRETER(AWAIT) {
        oop::Future* future = frame[args.extBC.a].obj->asFuture();
        if (future->ready) {
            frame[args.extBC.bc] = future->value;
            return InterpreterMessage::NoRequest();
        }
        return InterpreterMessage::AwaitFuture(future);
    }

    DEFINE_INTERPRETER(YIELD) {
        frame.ip()++;
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
            REGISTER_INTERPRETER(LOADB);
            REGISTER_INTERPRETER(LOADS);
            REGISTER_INTERPRETER(ADD);
            REGISTER_INTERPRETER(SUB);
            REGISTER_INTERPRETER(MUL);
            REGISTER_INTERPRETER(SDIV);
            REGISTER_INTERPRETER(UDIV);
            REGISTER_INTERPRETER(SMOD);
            REGISTER_INTERPRETER(UMOD);
            REGISTER_INTERPRETER(NEG);
            REGISTER_INTERPRETER(ABS);
            REGISTER_INTERPRETER(AND);
            REGISTER_INTERPRETER(OR);
            REGISTER_INTERPRETER(XOR);
            REGISTER_INTERPRETER(NOT);
            REGISTER_INTERPRETER(SHL);
            REGISTER_INTERPRETER(SHR);
            REGISTER_INTERPRETER(SAR);
            REGISTER_INTERPRETER(FADD);
            REGISTER_INTERPRETER(FSUB);
            REGISTER_INTERPRETER(FMUL);
            REGISTER_INTERPRETER(FDIV);
            REGISTER_INTERPRETER(FNEG);
            REGISTER_INTERPRETER(FABS);
            REGISTER_INTERPRETER(DADD);
            REGISTER_INTERPRETER(DSUB);
            REGISTER_INTERPRETER(DMUL);
            REGISTER_INTERPRETER(DDIV);
            REGISTER_INTERPRETER(DNEG);
            REGISTER_INTERPRETER(DABS);
            REGISTER_INTERPRETER(TR8);
            REGISTER_INTERPRETER(TR8S);
            REGISTER_INTERPRETER(TR16);
            REGISTER_INTERPRETER(TR16S);
            REGISTER_INTERPRETER(TR32);
            REGISTER_INTERPRETER(TR32S);
            REGISTER_INTERPRETER(SEX8);
            REGISTER_INTERPRETER(SEX16);
            REGISTER_INTERPRETER(SEX32);
            REGISTER_INTERPRETER(ZEX8);
            REGISTER_INTERPRETER(ZEX16);
            REGISTER_INTERPRETER(ZEX32);
            REGISTER_INTERPRETER(I2F);
            REGISTER_INTERPRETER(I2D);
            REGISTER_INTERPRETER(F2I);
            REGISTER_INTERPRETER(D2I);
            REGISTER_INTERPRETER(F2D);
            REGISTER_INTERPRETER(D2F);
            REGISTER_INTERPRETER(ICMP);
            REGISTER_INTERPRETER(UCMP);
            REGISTER_INTERPRETER(FCMP);
            REGISTER_INTERPRETER(DCMP);
            REGISTER_INTERPRETER(STRCMP);
            REGISTER_INTERPRETER(INC);
            REGISTER_INTERPRETER(DEC);
            REGISTER_INTERPRETER(JMP);
            REGISTER_INTERPRETER(JEQ);
            REGISTER_INTERPRETER(JNE);
            REGISTER_INTERPRETER(JLT);
            REGISTER_INTERPRETER(JLE);
            REGISTER_INTERPRETER(JGT);
            REGISTER_INTERPRETER(JGE);
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
                    return SchedulerMessage::Yielded();
                case InterpreterMessageType::AwaitFuture:
                    return SchedulerMessage::Awaiting(message.future);
            }
        }
    }
}
