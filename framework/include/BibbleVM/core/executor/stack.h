// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_CORE_EXECUTOR_STACK_H
#define BIBBLEVM_CORE_EXECUTOR_STACK_H 1

#include "BibbleVM/allocator/arena.h"

#include "BibbleVM/compiler/aot_trap.h"

#include "BibbleVM/core/executor/function.h"

#include "BibbleVM/core/value.h"

#include "BibbleVM/api.h"

namespace bibblevm::executor {
    class Function;
    struct Instruction;

    class BIBBLEVM_EXPORT Frame {
        friend SchedulerMessage compiler::AOTCompileTrap(VM& vm, executor::Frame& frame, executor::Task* task);
    public:
        Frame(Snapshot<GrowingArenaAllocator> arena, Frame* prev, Function& function, uint16_t registerCount, Value* returnRegister);

        Snapshot<GrowingArenaAllocator>& arena() { return mArena; }
        Frame* getPrev() const { return mPrev; }
        Module& getModule() const;
        Function& getFunction() const { return mFunction; }
        EntryPoint getEntryPoint() const { return mEntryPoint; }
        Instruction*& ip() { return mIP; }
        uintptr_t& checkpoint() { return mCheckpoint; }
        Value* getRegisters() const { return mRegisters; }
        Value* returnRegister() const { return mReturnRegister; }

        Value& operator[](uint16_t index) const { return mRegisters[index]; }

    private:
        Snapshot<GrowingArenaAllocator> mArena;

        Frame* mPrev;

        Function& mFunction;
        EntryPoint mEntryPoint;

        union {
            Instruction* mIP;
            uintptr_t mCheckpoint;
        };

        Value* mRegisters;
        Value* mReturnRegister; // pointer to single register which is where eventual return value will go
    };

    class BIBBLEVM_EXPORT Stack {
    public:
        Stack();
        Stack(Stack&& other) noexcept;

        Frame* getTop() const { return mTop; }

        Frame& pushFrame(Function& function, Value* returnRegister);
        void popFrame();

    private:
        GrowingArenaAllocator mArena;

        Frame* mTop;
    };
}

#endif // BIBBLEVM_CORE_EXECUTOR_STACK_H
