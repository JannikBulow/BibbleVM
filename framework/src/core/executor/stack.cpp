// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/core/executor/function.h"
#include "BibbleVM/core/executor/stack.h"

namespace bibblevm::executor {
    Frame::Frame(Snapshot<GrowingArenaAllocator> arena, Frame* prev, Function& function, uint16_t registerCount)
        : mArena(std::move(arena))
        , mPrev(prev)
        , mFunction(function)
        , mIP(function.getInstructions())
        , mRegisters(mArena->allocate<Value>(registerCount)) {}

    Stack::Stack()
        : mArena(GrowingArenaAllocator::Create(16 * 1024))
        , mTop(nullptr) {}

    Frame& Stack::pushFrame(Function& function) {
        Snapshot snapshot = mArena;
        auto* newTop = snapshot->allocate<Frame>(1);
        new (newTop) Frame(std::move(snapshot), mTop, function, function.getRegisterCount());
        mTop = newTop;
        return *mTop;
    }

    void Stack::popFrame() {
        Frame* newTop = mTop->getPrev();
        mTop->~Frame(); // You know it's good code when a destructor is manually called
        mTop = newTop;
    }
}
