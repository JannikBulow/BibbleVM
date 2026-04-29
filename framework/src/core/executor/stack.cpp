// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/core/executor/function.h"
#include "BibbleVM/core/executor/stack.h"

namespace bibblevm::executor {
    Frame::Frame(Snapshot<GrowingArenaAllocator> arena, Frame* prev, Function& function, uint16_t registerCount, Value* returnRegister)
        : mArena(std::move(arena))
        , mPrev(prev)
        , mFunction(function)
        , mIP(function.getInstructions())
        , mRegisters(mArena->allocate<Value>(registerCount))
        , mReturnRegister(returnRegister) {}

    Module& Frame::getModule() const {
        return mFunction.getModule();
    }

    Stack::Stack()
        : mArena(GrowingArenaAllocator::Create(16 * 1024))
        , mTop(nullptr) {}

    Stack::Stack(Stack&& other) noexcept
        : mArena(std::move(other.mArena))
        , mTop(other.mTop) {
        other.mTop = nullptr;
    }

    Frame& Stack::pushFrame(Function& function, Value* returnRegister) {
        Snapshot snapshot = mArena;
        Frame* newTop = snapshot->allocate<Frame>(1);
        new (newTop) Frame(std::move(snapshot), mTop, function, function.getRegisterCount(), returnRegister);
        mTop = newTop;
        return *mTop;
    }

    void Stack::popFrame() {
        Frame* newTop = mTop->getPrev();
        mTop->~Frame(); // You know it's good code when a destructor is manually called
        mTop = newTop;
    }
}
