// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_CORE_EXECUTOR_STACK_H
#define BIBBLEVM_CORE_EXECUTOR_STACK_H 1

#include "BibbleVM/allocator/arena.h"

#include "BibbleVM/core/value.h"

#include "BibbleVM/api.h"

namespace bibblevm::executor {
    class Function;
    struct Instruction;

    class BIBBLEVM_EXPORT Frame {
    public:
        Frame(Snapshot<GrowingArenaAllocator> arena, Frame* prev, Function& function, uint16_t registerCount);

        Frame* getPrev() const { return mPrev; }
        Function& getFunction() const { return mFunction; }
        Instruction*& ip() { return mIP; }

        Value& operator[](uint16_t index) const { return mRegisters[index]; }

    private:
        Snapshot<GrowingArenaAllocator> mArena;

        Frame* mPrev;

        Function& mFunction;
        Instruction* mIP;
        Value* mRegisters;
    };

    class BIBBLEVM_EXPORT Stack {
    public:
        Stack();

        Frame* getTop() const { return mTop; }

        Frame& pushFrame(Function& function);
        void popFrame();

    private:
        GrowingArenaAllocator mArena;

        Frame* mTop;
    };
}

#endif // BIBBLEVM_CORE_EXECUTOR_STACK_H
