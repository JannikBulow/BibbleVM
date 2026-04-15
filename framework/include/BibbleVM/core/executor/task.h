// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_CORE_EXECUTOR_TASK_H
#define BIBBLEVM_CORE_EXECUTOR_TASK_H 1

#include "BibbleVM/core/executor/stack.h"

#include "BibbleVM/core/oop/object.h"

namespace bibblevm::executor {
    class Scheduler;
    struct Task;

    struct Task {
        Stack stack;

        uint8_t priority = 0;

        oop::Future* completionFuture;
        oop::Future* waitingOn = nullptr;

        // Result of the latest function return in this tasks call stack
        Value result;

        Scheduler* scheduler;
    };
}

#endif // BIBBLEVM_CORE_EXECUTOR_TASK_H
