// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/core/executor/scheduler.h"

#include "BibbleVM/core/oop/array_view.h"
#include "BibbleVM/core/oop/object.h"

#include "BibbleVM/vm.h"

namespace bibblevm::oop {
    void Future::addWaiter(VM& vm, executor::Task* waiter) {
        GrowingArrayView<executor::Task*> waitersView(vm.memoryManager(), waiters, waiterCount);
        waitersView.add(waiter);
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
}
