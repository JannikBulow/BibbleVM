// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/core/opcodes.h"

#include "BibbleVM/core/executor/function.h"
#include "BibbleVM/core/executor/instruction.h"
#include "BibbleVM/core/executor/interpreter.h"

#include "BibbleVM/util/safe_math.h"

#include "BibbleVM/vm.h"

#define DEFINE_INTERPRETER(opcode) InterpreterMessage Interpret##opcode(VM& vm, Frame& frame, Task* task, const InstructionArguments& args)
#define REGISTER_INTERPRETER(opcode) table[opcode] = Interpret##opcode

#define NONZERO 69

namespace bibblevm::executor {
    DEFINE_INTERPRETER(NOP) {
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(MOV) {
        frame[args.a] = frame[args.b];
        return InterpreterMessage::Continue();
    }


    DEFINE_INTERPRETER(MOV_RANGE) {
        memmove(&frame[args.a], &frame[args.b], args.c * sizeof(Value));
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(SWAP) {
        Value tmp = frame[args.a];
        frame[args.a] = frame[args.b];
        frame[args.b] = tmp;
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(LOAD_CONST) {
        frame[args.a] = frame.getFunction().mergedConstPool().get(args.b);
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(LOAD_IMM) {
        frame[args.a].ul = args.b;
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(ADD) {
        frame[args.a].l = frame[args.b].l + frame[args.c].l;
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(SUB) {
        frame[args.a].l = frame[args.b].l - frame[args.c].l;
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(MUL) {
        frame[args.a].l = frame[args.b].l * frame[args.c].l;
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(SDIV) {
        frame[args.a].l = frame[args.b].l / frame[args.c].l;
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(UDIV) {
        frame[args.a].ul = frame[args.b].ul / frame[args.c].ul;
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(SMOD) {
        frame[args.a].l = frame[args.b].l % frame[args.c].l;
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(UMOD) {
        frame[args.a].ul = frame[args.b].ul % frame[args.c].ul;
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(NEG) {
        frame[args.a].l = -frame[args.b].l;
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(ABS) {
        frame[args.a].l = std::abs(frame[args.b].l);
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(AND) {
        frame[args.a].ul = frame[args.b].ul & frame[args.c].ul;
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(OR) {
        frame[args.a].ul = frame[args.b].ul | frame[args.c].ul;
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(XOR) {
        frame[args.a].ul = frame[args.b].ul ^ frame[args.c].ul;
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(NOT) {
        frame[args.a].ul = ~frame[args.b].ul;
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(SHL) {
        frame[args.a].ul = math::LogicalShiftLeft(frame[args.b].ul, frame[args.c].ui);
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(SHR) {
        frame[args.a].ul = math::LogicalShiftRight(frame[args.b].ul, frame[args.c].ui);
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(SAR) {
        frame[args.a].l = math::ArithmeticShiftRight(frame[args.b].l, frame[args.c].ui);
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(FADD) {
        frame[args.a].f = frame[args.b].f + frame[args.c].f;
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(FSUB) {
        frame[args.a].f = frame[args.b].f - frame[args.c].f;
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(FMUL) {
        frame[args.a].f = frame[args.b].f * frame[args.c].f;
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(FDIV) {
        frame[args.a].f = frame[args.b].f / frame[args.c].f;
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(FNEG) {
        frame[args.a].f = -frame[args.b].f;
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(FABS) {
        frame[args.a].f = std::abs(frame[args.b].f);
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(TR8) {
        frame[args.a].ub = static_cast<UByte>(frame[args.b].ul);
        return InterpreterMessage::Continue();
    }


    DEFINE_INTERPRETER(TR16) {
        frame[args.a].us = static_cast<UShort>(frame[args.b].ul);
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(TR32) {
        frame[args.a].ui = static_cast<UInt>(frame[args.b].ul);
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(SEX8) {
        frame[args.a].l = static_cast<Long>(frame[args.b].b);
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(SEX16) {
        frame[args.a].l = static_cast<Long>(frame[args.b].s);
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(SEX32) {
        frame[args.a].l = static_cast<Long>(frame[args.b].i);
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(ZEX8) {
        frame[args.a].ul = static_cast<ULong>(frame[args.b].ub);
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(ZEX16) {
        frame[args.a].ul = static_cast<ULong>(frame[args.b].us);
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(ZEX32) {
        frame[args.a].ul = static_cast<ULong>(frame[args.b].ui);
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(I2F) {
        frame[args.a].f = math::IntToFP<float, Long>(frame[args.b].l);
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(U2F) {
        frame[args.a].f = math::IntToFP<float, ULong>(frame[args.b].ul);
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(I2D) {
        frame[args.a].d = math::IntToFP<double, Long>(frame[args.b].l);
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(U2D) {
        frame[args.a].d = math::IntToFP<double, ULong>(frame[args.b].ul);
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(F2I) {
        frame[args.a].l = math::FPToInt<float, Long>(frame[args.b].f);
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(F2U) {
        frame[args.a].ul = math::FPToInt<float, ULong>(frame[args.b].f);
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(D2I) {
        frame[args.a].l = math::FPToInt<double, Long>(frame[args.b].d);
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(D2U) {
        frame[args.a].ul = math::FPToInt<double, ULong>(frame[args.b].d);
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(F2D) {
        frame[args.a].d = static_cast<double>(frame[args.b].f);
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(D2F) {
        frame[args.a].f = static_cast<float>(frame[args.b].d);
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(ICMP) {
        Long& dst = frame[args.a].l;
        Long lhs = frame[args.b].l;
        Long rhs = frame[args.c].l;

        if (lhs < rhs) dst = -1;
        if (lhs > rhs) dst = 1;
        else dst = 0;

        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(UCMP) {
        Long& dst = frame[args.a].l;
        ULong lhs = frame[args.b].ul;
        ULong rhs = frame[args.c].ul;

        if (lhs < rhs) dst = -1;
        if (lhs > rhs) dst = 1;
        else dst = 0;

        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(FCMP) {
        Long& dst = frame[args.a].l;
        float lhs = frame[args.b].f;
        float rhs = frame[args.c].f;

        if (lhs < rhs) dst = -1;
        if (lhs > rhs) dst = 1;
        else dst = 0;

        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(STRCMP) {
        String str1 = frame[args.b].obj->asString();
        String str2 = frame[args.c].obj->asString();
        frame[args.a].l = static_cast<Long>(str1.compareCorrect(str2));
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(INC) {
        frame[args.a].l += static_cast<Long>(args.b);
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(DEC) {
        frame[args.a].l -= static_cast<Long>(args.b);
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(JMP) {
        return InterpreterMessage::Branch(args.a);
    }

    DEFINE_INTERPRETER(JEQ) {
        if (frame[args.a].l == 0) return InterpreterMessage::Branch(static_cast<int32_t>(args.b));
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(JNE) {
        if (frame[args.a].l != 0) return InterpreterMessage::Branch(static_cast<int32_t>(args.b));
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(JLT) {
        if (frame[args.a].l < 0) return InterpreterMessage::Branch(static_cast<int32_t>(args.b));
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(JLE) {
        if (frame[args.a].l <= 0) return InterpreterMessage::Branch(static_cast<int32_t>(args.b));
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(JGT) {
        if (frame[args.a].l > 0) return InterpreterMessage::Branch(static_cast<int32_t>(args.b));
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(JGE) {
        if (frame[args.a].l >= 0) return InterpreterMessage::Branch(static_cast<int32_t>(args.b));
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(NEWINSTANCE) {
        frame[args.a].obj = vm.memoryManager().allocateInstance(vm, frame.getFunction().mergedConstPool().get(args.b).ci);
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(NEWARRAY) {
        frame[args.a].obj = vm.memoryManager().allocateArray(vm, static_cast<oop::Type>(args.c), frame[args.b].ul);
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(NEWSTRING) {
        oop::Object* data = frame[args.b].obj;
        if (data == nullptr) return InterpreterMessage::Errored(Error::NULL_REFERENCE);
        if (data->kind != oop::ObjectKind::Array) return InterpreterMessage::Errored(Error::INVALID_OBJECT_KIND);

        frame[args.a].obj = vm.memoryManager().allocateString(vm, std::string_view(reinterpret_cast<const char*>(data->asArray()->elementBytes), data->asArray()->length));

        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(NEWFUTURE) {
        frame[args.a].obj = vm.memoryManager().allocateFuture(vm);
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(OBJKIND) {
        oop::Object* object = frame[args.b].obj;
        if (object == nullptr) return InterpreterMessage::Errored(Error::NULL_REFERENCE);

        frame[args.a].ul = static_cast<ULong>(object->kind);

        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(ISKIND) {
        oop::Object* object = frame[args.b].obj;
        if (object == nullptr) return InterpreterMessage::Errored(Error::NULL_REFERENCE);

        if (static_cast<uint8_t>(object->kind) != args.c) {
            frame[args.a].ul = 0;
        } else {
            frame[args.a].ul = NONZERO;
        }

        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(INSTANCEOF) {
        oop::Object* object = frame[args.b].obj;
        if (object == nullptr || object->kind != oop::ObjectKind::Instance) {
            frame[args.a].ul = NONZERO;
            return InterpreterMessage::Continue();
        }

        if (object->asInstance()->clas->isAssignableTo(frame.getFunction().mergedConstPool().get(args.c).ci)) {
            frame[args.a].ul = 0;
        } else {
            frame[args.a].ul = NONZERO;
        }

        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(GETFIELD) {
        oop::Object* object = frame[args.b].obj;
        oop::Field* field = frame.getFunction().mergedConstPool().get(args.c).fi;

        if (object == nullptr) return InterpreterMessage::Errored(Error::NULL_REFERENCE);
        if (object->kind != oop::ObjectKind::Instance) return InterpreterMessage::Errored(Error::INVALID_OBJECT_KIND);

        frame[args.a] = object->asInstance()->clas->readField(object->asInstance(), field);

        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(SETFIELD) {
        oop::Object* object = frame[args.a].obj;
        oop::Field* field = frame.getFunction().mergedConstPool().get(args.b).fi;

        if (object == nullptr) return InterpreterMessage::Errored(Error::NULL_REFERENCE);
        if (object->kind != oop::ObjectKind::Instance) return InterpreterMessage::Errored(Error::INVALID_OBJECT_KIND);

        object->asInstance()->clas->writeField(object->asInstance(), field, frame[args.c]);

        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(DISPATCHMETHOD) {
        oop::Object* object = frame[args.b].obj;
        oop::Method* method = frame.getFunction().mergedConstPool().get(args.c).mei;

        if (object == nullptr) return InterpreterMessage::Errored(Error::NULL_REFERENCE);
        if (object->kind != oop::ObjectKind::Instance) return InterpreterMessage::Errored(Error::INVALID_OBJECT_KIND);

        frame[args.a].fni = object->asInstance()->clas->dispatchMethod(method);

        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(GETCLASS) {
        oop::Object* object = frame[args.b].obj;

        if (object == nullptr) return InterpreterMessage::Errored(Error::NULL_REFERENCE);
        if (object->kind != oop::ObjectKind::Instance) return InterpreterMessage::Errored(Error::INVALID_OBJECT_KIND);

        frame[args.a].ci = object->asInstance()->clas;

        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(ARRAYLENGTH) {
        oop::Object* object = frame[args.b].obj;

        if (object == nullptr) return InterpreterMessage::Errored(Error::NULL_REFERENCE);
        if (object->kind != oop::ObjectKind::Array) return InterpreterMessage::Errored(Error::INVALID_OBJECT_KIND);

        frame[args.a].ul = object->asArray()->length;

        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(ARRAYGET) {
        return InterpreterMessage::Errored(Error::USERLAND);
    }

    DEFINE_INTERPRETER(ARRAYSET) {
        return InterpreterMessage::Errored(Error::USERLAND);
    }

    DEFINE_INTERPRETER(STRLENGTH) {
        oop::Object* object = frame[args.b].obj;

        if (object == nullptr) return InterpreterMessage::Errored(Error::NULL_REFERENCE);
        if (object->kind != oop::ObjectKind::String) return InterpreterMessage::Errored(Error::INVALID_OBJECT_KIND);

        frame[args.a].ul = object->asString()->lengthBytes;

        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(STRGET) {
        oop::Object* object = frame[args.b].obj;

        if (object == nullptr) return InterpreterMessage::Errored(Error::NULL_REFERENCE);
        if (object->kind != oop::ObjectKind::String) return InterpreterMessage::Errored(Error::INVALID_OBJECT_KIND);

        frame[args.a].ul = static_cast<ULong>(object->asString()->bytes[frame[args.c].ul]);

        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(STR2ARRAY) {
        oop::Object* object = frame[args.b].obj;

        if (object == nullptr) return InterpreterMessage::Errored(Error::NULL_REFERENCE);
        if (object->kind != oop::ObjectKind::String) return InterpreterMessage::Errored(Error::INVALID_OBJECT_KIND);

        oop::Object* array = vm.memoryManager().allocateArray(vm, oop::Type::UByte, object->asString()->lengthBytes);
        if (array == nullptr) return InterpreterMessage::Continue(); //TODO: error or try gc

        memcpy(array->asArray()->elementBytes, object->asString()->bytes, object->asString()->lengthBytes);

        frame[args.a].obj = array;

        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(RESOLVE) {
        oop::Object* object = frame[args.a].obj;

        if (object == nullptr) return InterpreterMessage::Errored(Error::NULL_REFERENCE);
        if (object->kind != oop::ObjectKind::Future) return InterpreterMessage::Errored(Error::INVALID_OBJECT_KIND);
        if (object->asFuture()->ready) return InterpreterMessage::Errored(Error::INVALID_STATE);

        object->asFuture()->complete(vm, frame[args.b]);

        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(CANCEL) {
        oop::Object* object = frame[args.a].obj;

        if (object == nullptr) return InterpreterMessage::Errored(Error::NULL_REFERENCE);
        if (object->kind != oop::ObjectKind::Future) return InterpreterMessage::Errored(Error::INVALID_OBJECT_KIND);
        if (object->asFuture()->ready || object->asFuture()->cancelled) return InterpreterMessage::Errored(Error::INVALID_STATE);

        object->asFuture()->cancel(vm, Error::USERLAND, frame[args.b].obj);

        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(ISFUTUREREADY) {
        oop::Object* object = frame[args.b].obj;

        if (object == nullptr) return InterpreterMessage::Errored(Error::NULL_REFERENCE);
        if (object->kind != oop::ObjectKind::Future) return InterpreterMessage::Errored(Error::INVALID_OBJECT_KIND);

        if (object->asFuture()->ready) {
            frame[args.a].ul = 0;
        } else {
            frame[args.a].ul = NONZERO;
        }

        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(POLL) {
        oop::Object* object = frame[args.b].obj;

        if (object == nullptr) return InterpreterMessage::Errored(Error::NULL_REFERENCE);
        if (object->kind != oop::ObjectKind::Future) return InterpreterMessage::Errored(Error::INVALID_OBJECT_KIND);

        if (object->asFuture()->ready) {
            frame[args.b] = object->asFuture()->value;
            frame[args.a].ul = 0;
        } else if (object->asFuture()->cancelled) {
            frame[args.b] = object->asFuture()->value;
            frame[args.a].ul = 1;
        } else {
            frame[args.a].ul = -1;
        }

        return InterpreterMessage::Continue();
    }

    // This interpreter gets called again once the future is completed if it wasn't completed before reaching this.
    // That design is technically a little inefficient, but it's way easier and better looking than the alternatives.
    DEFINE_INTERPRETER(AWAIT) {
        oop::Object* object = frame[args.b].obj;

        if (object == nullptr) return InterpreterMessage::Errored(Error::NULL_REFERENCE);
        if (object->kind != oop::ObjectKind::Future) return InterpreterMessage::Errored(Error::INVALID_OBJECT_KIND);

        oop::Future* future = object->asFuture();
        if (future->cancelled) {
            return InterpreterMessage::Errored(static_cast<Error::Type>(future->error.type), future->error.message->asString());
        }

        if (future->ready) {
            frame[args.a] = future->value;
            return InterpreterMessage::Continue(); // or yield?
        }
        return InterpreterMessage::AwaitFuture(future);
    }

    DEFINE_INTERPRETER(CALL) {
        return InterpreterMessage::CallFunction(frame.getFunction().mergedConstPool().get(args.b).fni, args.a, args.c);
    }

    DEFINE_INTERPRETER(TAIL_CALL) {
        return InterpreterMessage::Errored(Error::USERLAND); // TODO: implement
    }

    DEFINE_INTERPRETER(CALLA) {
        Task* newTask = vm.scheduler().schedule(vm, *frame.getFunction().mergedConstPool().get(args.b).fni, task->priority, &frame[args.c]);
        frame[args.a].obj = newTask->completionFuture->asObject();
        frame[args.a].isObject = true;
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(CALLAP) {
        Task* newTask = vm.scheduler().schedule(vm, *frame.getFunction().mergedConstPool().get(args.c).fni, static_cast<uint8_t>(frame[args.b].ul), &frame[args.d]);
        frame[args.a].obj = newTask->completionFuture->asObject();
        frame[args.a].isObject = true;
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(CALLARP) {
        Task* newTask = vm.scheduler().schedule(vm, *frame.getFunction().mergedConstPool().get(args.c).fni, task->priority + static_cast<uint8_t>(args.b), &frame[args.d]);
        frame[args.a].obj = newTask->completionFuture->asObject();
        frame[args.a].isObject = true;
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(CALL_DYN) {
        return InterpreterMessage::CallFunction(frame[args.b].fni, args.a, args.c);
    }

    DEFINE_INTERPRETER(TAIL_CALL_DYN) {
        return InterpreterMessage::Errored(Error::USERLAND); // TODO: implement
    }

    DEFINE_INTERPRETER(CALLA_DYN) {
        Task* newTask = vm.scheduler().schedule(vm, *frame[args.b].fni, task->priority, &frame[args.c]);
        frame[args.a].obj = newTask->completionFuture->asObject();
        frame[args.a].isObject = true;
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(CALLAP_DYN) {
        Task* newTask = vm.scheduler().schedule(vm, *frame[args.b].fni, static_cast<uint8_t>(frame[args.b].ul), &frame[args.d]);
        frame[args.a].obj = newTask->completionFuture->asObject();
        frame[args.a].isObject = true;
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(CALLARP_DYN) {
        Task* newTask = vm.scheduler().schedule(vm, *frame[args.b].fni, task->priority + static_cast<uint8_t>(args.b), &frame[args.d]);
        frame[args.a].obj = newTask->completionFuture->asObject();
        frame[args.a].isObject = true;
        return InterpreterMessage::Continue();
    }

    DEFINE_INTERPRETER(RETURN) {
        return InterpreterMessage::ReturnFromFunction(args.a);
    }

    DEFINE_INTERPRETER(YIELD) {
        frame.ip()++;
        return InterpreterMessageType::YieldExecution;
    }

    Interpreter GetInterpreter(VM& vm, Opcode opcode) {
        static const std::array<Interpreter, 256> table = [] {
            std::array<Interpreter, 256> table{};
            REGISTER_INTERPRETER(NOP);
            REGISTER_INTERPRETER(MOV);
            REGISTER_INTERPRETER(MOV_RANGE);
            REGISTER_INTERPRETER(SWAP);
            REGISTER_INTERPRETER(LOAD_CONST);
            REGISTER_INTERPRETER(LOAD_IMM);
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
            REGISTER_INTERPRETER(TR8);
            REGISTER_INTERPRETER(TR16);
            REGISTER_INTERPRETER(TR32);
            REGISTER_INTERPRETER(SEX8);
            REGISTER_INTERPRETER(SEX16);
            REGISTER_INTERPRETER(SEX32);
            REGISTER_INTERPRETER(ZEX8);
            REGISTER_INTERPRETER(ZEX16);
            REGISTER_INTERPRETER(ZEX32);
            REGISTER_INTERPRETER(I2F);
            REGISTER_INTERPRETER(U2F);
            REGISTER_INTERPRETER(I2D);
            REGISTER_INTERPRETER(U2D);
            REGISTER_INTERPRETER(F2I);
            REGISTER_INTERPRETER(F2U);
            REGISTER_INTERPRETER(D2I);
            REGISTER_INTERPRETER(D2U);
            REGISTER_INTERPRETER(F2D);
            REGISTER_INTERPRETER(D2F);
            REGISTER_INTERPRETER(ICMP);
            REGISTER_INTERPRETER(UCMP);
            REGISTER_INTERPRETER(FCMP);
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
            REGISTER_INTERPRETER(NEWINSTANCE);
            REGISTER_INTERPRETER(NEWARRAY);
            REGISTER_INTERPRETER(NEWSTRING);
            REGISTER_INTERPRETER(NEWFUTURE);
            REGISTER_INTERPRETER(OBJKIND);
            REGISTER_INTERPRETER(ISKIND);
            REGISTER_INTERPRETER(INSTANCEOF);
            REGISTER_INTERPRETER(GETFIELD);
            REGISTER_INTERPRETER(SETFIELD);
            REGISTER_INTERPRETER(DISPATCHMETHOD);
            REGISTER_INTERPRETER(GETCLASS);
            REGISTER_INTERPRETER(ARRAYLENGTH);
            REGISTER_INTERPRETER(ARRAYGET);
            REGISTER_INTERPRETER(ARRAYSET);
            REGISTER_INTERPRETER(STRLENGTH);
            REGISTER_INTERPRETER(STRGET);
            REGISTER_INTERPRETER(STR2ARRAY);
            REGISTER_INTERPRETER(RESOLVE);
            REGISTER_INTERPRETER(CANCEL);
            REGISTER_INTERPRETER(ISFUTUREREADY);
            REGISTER_INTERPRETER(POLL);
            REGISTER_INTERPRETER(AWAIT);
            REGISTER_INTERPRETER(CALL);
            REGISTER_INTERPRETER(TAIL_CALL);
            REGISTER_INTERPRETER(CALLA);
            REGISTER_INTERPRETER(CALLAP);
            REGISTER_INTERPRETER(CALLARP);
            REGISTER_INTERPRETER(CALL_DYN);
            REGISTER_INTERPRETER(TAIL_CALL_DYN);
            REGISTER_INTERPRETER(CALLA_DYN);
            REGISTER_INTERPRETER(CALLAP_DYN);
            REGISTER_INTERPRETER(CALLARP_DYN);
            REGISTER_INTERPRETER(RETURN);
            REGISTER_INTERPRETER(YIELD);
            return table;
        }();
        return table[opcode];
    }

    SchedulerMessage BytecodeInterpreter(VM& vm, Frame& frame, Task* task) {
        Instruction*& instruction = frame.ip();
        while (true) {
            InterpreterMessage message = instruction->interpreter(vm, frame, task, instruction->args);
            switch (message.type) {
                case InterpreterMessageType::Continue:
                    instruction++;
                    break;
                case InterpreterMessageType::Branch:
                    instruction += message.branch + 1;
                    break;
                case InterpreterMessageType::Errored:
                    return SchedulerMessage::Errored(message.error.type, message.error.message);
                case InterpreterMessageType::CallFunction:
                    instruction++;
                    return SchedulerMessage::Called(message.call.function, message.call.destinationRegister, message.call.argsBegin);
                case InterpreterMessageType::ReturnFromFunction:
                    return SchedulerMessage::Returned(frame[message.returnRegister]);
                case InterpreterMessageType::YieldExecution:
                    return SchedulerMessage::Yielded();
                case InterpreterMessageType::AwaitFuture:
                    return SchedulerMessage::Awaiting(message.future);
            }
        }
    }

    SchedulerMessage AutoYieldingBytecodeInterpreter(VM& vm, Frame& frame, Task* task) {
        Instruction*& instruction = frame.ip();
        size_t executedInstructions = 0;
        while (true) {
            InterpreterMessage message = instruction->interpreter(vm, frame, task, instruction->args);
            switch (message.type) {
                case InterpreterMessageType::Continue:
                    instruction++;
                    break;
                case InterpreterMessageType::Branch:
                    instruction += message.branch + 1;
                    break;
                case InterpreterMessageType::Errored:
                    return SchedulerMessage::Errored(message.error.type, message.error.message);
                case InterpreterMessageType::CallFunction:
                    instruction++;
                    return SchedulerMessage::Called(message.call.function, message.call.destinationRegister, message.call.argsBegin);
                case InterpreterMessageType::ReturnFromFunction:
                    return SchedulerMessage::Returned(frame[message.returnRegister]);
                case InterpreterMessageType::YieldExecution:
                    return SchedulerMessage::Yielded();
                case InterpreterMessageType::AwaitFuture:
                    return SchedulerMessage::Awaiting(message.future);
            }

            if (++executedInstructions >= vm.config().scheduler.autoYielding.threshold) {
                return SchedulerMessage::Yielded();
            }
        }
    }
}
