// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_CORE_EXECUTOR_SCHEDULER_H
#define BIBBLEVM_CORE_EXECUTOR_SCHEDULER_H 1

#include "BibbleVM/core/executor/function.h"
#include "BibbleVM/core/executor/task.h"

#include <deque>
#include <vector>

namespace bibblevm::executor {
    class BIBBLEVM_EXPORT Scheduler {
    public:
        std::vector<Task*>& allTasks() { return mAllTasks; }

        void setGCRunning(bool value) { mGCRunning = value; }
        void safeDeleteExpiredTasks();

        void enqueue(Task* task);

        Task* schedule(VM& vm, Function& function, Value* arguments);

        void run(VM& vm);

    private:
        std::vector<Task*> mAllTasks; // easier way to store all stacks for the MemoryManager to scan
        std::vector<Task*> mExpiredTasks; // allTasks is append-only during a GC cycle. even between. this is so we don't accidentally miss some objects because of the index
        std::deque<Task*> mReadyQueue;
        bool mGCRunning = false;

        void deleteTask(Task* task);
    };
}

#endif // BIBBLEVM_CORE_EXECUTOR_SCHEDULER_H
