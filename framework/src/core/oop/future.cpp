// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/core/executor/scheduler.h"

#include "BibbleVM/core/oop/array_view.h"
#include "BibbleVM/core/oop/object.h"

#include "BibbleVM/vm.h"

namespace bibblevm::oop {
    void Future::addWaiter(VM& vm, executor::Task* waiter) {
        GrowingArrayView<executor::Task*> waitersView(vm.memoryManager(), waiters, waiterCount);
        waiter->waitingOn = this;
        waitersView.add(vm, waiter);
    }

    void Future::complete(VM& vm, Value value) {
        GrowingArrayView<executor::Task*> waitersView(vm.memoryManager(), waiters, waiterCount);

        ready = true;
        this->value = value;

        for (executor::Task* task : waitersView) {
            task->waitingOn = nullptr;
            task->scheduler->enqueue(task);
        }

        waitersView.clear();
    }

    void Future::cancel(VM& vm, int type, Object* message) {
        cancelled = true;
        error.type = type;
        if (message != nullptr && message->kind == ObjectKind::String) {
            error.message = message;
        } else {
            error.message = nullptr;
        }

        //TODO: implement error logic
    }
}
