// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/executor/scheduler.h"
#include "BibbleVM/executor/task.h"

namespace bibblevm::executor {
    void Future::complete(Value value) {
        ready = true;
        this->value = value;

        for (Task* task : waiters) {
            task->waitingOn = nullptr;
            task->scheduler->enqueue(task);
        }

        waiters.clear();
    }
}
