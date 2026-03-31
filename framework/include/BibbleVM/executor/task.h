// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_EXECUTOR_TASK_H
#define BIBBLEVM_EXECUTOR_TASK_H 1

#include "BibbleVM/executor/stack.h"

namespace bibblevm::executor {
    class Scheduler;
    struct Task;

    struct BIBBLEVM_EXPORT Future {
        bool ready = false;
        Value value;

        std::vector<Task*> waiters;

        void complete(Value value);
    };

    struct Task {
        Stack stack;

        Future* completionFuture;
        Future* waitingOn = nullptr;

        // Result of the latest function return in this tasks call stack
        Value result;

        Scheduler* scheduler;
    };
}

#endif // BIBBLEVM_EXECUTOR_TASK_H
