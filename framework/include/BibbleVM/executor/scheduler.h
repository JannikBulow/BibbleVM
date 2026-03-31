// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_EXECUTOR_SCHEDULER_H
#define BIBBLEVM_EXECUTOR_SCHEDULER_H 1

#include "BibbleVM/executor/function.h"
#include "BibbleVM/executor/task.h"

#include <deque>

namespace bibblevm::executor {
    class BIBBLEVM_EXPORT Scheduler {
    public:
        void enqueue(Task* task);

        Task* schedule(Function& function);

        void run(VM& vm);

    private:
        std::deque<Task*> mReadyQueue;
    };
}

#endif // BIBBLEVM_EXECUTOR_SCHEDULER_H
