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
        explicit Scheduler(VM& vm);

        std::vector<Task*>& allTasks() { return mAllTasks; }

        bool isGCRunning() const { return mGCRunning; }
        void setGCRunning(bool value) { mGCRunning = value; }
        void safeDeleteExpiredTasks();

        void enqueue(Task* task);

        Task* schedule(VM& vm, Function& function, uint8_t priority, const Value* arguments);

        void run(VM& vm);

    private:
        std::vector<Task*> mAllTasks; // easier way to store all stacks for the MemoryManager to scan
        std::vector<Task*> mExpiredTasks; // allTasks is append-only during a GC cycle. even between. this is so we don't accidentally miss some objects because of the index

        std::vector<std::deque<Task*>> mReadyQueues;
        std::vector<uint8_t> mLevelSelection;
        size_t mLevelCursor = 0;

        bool mGCRunning = false;

        void deleteTask(Task* task);

        uint8_t pickLevel();
        Task* nextTask(VM& vm);
    };
}

#endif // BIBBLEVM_CORE_EXECUTOR_SCHEDULER_H
