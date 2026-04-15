// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/core/executor/scheduler.h"

#include "BibbleVM/vm.h"

#include <algorithm>

namespace bibblevm::executor {
    constexpr uint32_t PowI(uint8_t base, uint8_t exp) {
        uint32_t result = 1;
        for (uint8_t i = 0; i < exp; i++) {
            result *= base;
        }
        return result;
    }

    Scheduler::Scheduler(VM& vm) {
        uint8_t levels = std::max<uint8_t>(1, vm.config().scheduler.priority.levels);
        mReadyQueues.resize(levels);

        if (levels == 1 || vm.config().scheduler.priority.boost <= 1) {
            mLevelSelection.push_back(0);
            return;
        }

        std::vector<uint32_t> weights(levels);
        uint8_t maxLevel = levels - 1;

        for (uint8_t level = 0; level < levels; level++) {
            uint8_t distanceFromTop = maxLevel - level;
            weights[level] = PowI(vm.config().scheduler.priority.boost, distanceFromTop);
        }

        for (uint8_t level = 0; level < levels; level++) {
            for (uint32_t i = 0; i < weights[level]; i++) {
                mLevelSelection.push_back(level);
            }
        }
    }

    void Scheduler::safeDeleteExpiredTasks() {
        for (Task* task : mExpiredTasks) {
            std::erase(mAllTasks, task);
            delete task;
        }
        mExpiredTasks.clear();
    }

    void Scheduler::enqueue(Task* task) {
        if (mReadyQueues.size() == 1) {
            mReadyQueues[0].push_front(task);
            return;
        }

        uint8_t level = std::min(task->priority, static_cast<uint8_t>(mReadyQueues.size() - 1));
        mReadyQueues[level].push_back(task);
    }

    Task* Scheduler::schedule(VM& vm, Function& function, uint8_t priority, const Value* arguments) {
        Task* task = new Task(); // TODO: not this

        task->priority = priority;
        task->scheduler = this;

        Frame& frame = task->stack.pushFrame(function);
        memcpy(&frame[0], arguments, function.getParameterCount() * sizeof(Value));

        oop::Object* futureObject = vm.memoryManager().allocateFuture(vm);
        task->completionFuture = futureObject->asFuture();

        if (mAllTasks.size() + 1 > vm.config().scheduler.maxTasks) {
            return nullptr; // TODO:
        }

        mAllTasks.push_back(task);
        enqueue(task);

        return task;
    }

    static SchedulerMessage RunTask(VM& vm, Task& task) {
        while (task.stack.getTop() != nullptr) {
            Frame& frame = *task.stack.getTop();

            SchedulerMessage message = frame.getFunction().invoke(vm, frame, &task);
            switch (message.type) {
                case SchedulerMessageType::Errored: return message;
                case SchedulerMessageType::Called: {
                    Frame& newFrame = task.stack.pushFrame(*message.call.function);

                    for (uint16_t i = 0; i < message.call.function->getParameterCount(); i++) {
                        newFrame[i] = frame[message.call.argsBegin + i];
                    }

                    vm.memoryManager().safepoint(vm);

                    break;
                }
                case SchedulerMessageType::Returned: {
                    task.result = message.returnValue;
                    task.stack.popFrame();
                    if (task.stack.getTop() != nullptr) (*task.stack.getTop())[0] = message.returnValue;
                    vm.memoryManager().safepoint(vm);
                    break;
                }
                case SchedulerMessageType::Yielded:
                case SchedulerMessageType::Awaiting:
                    vm.memoryManager().safepoint(vm);
                    return message;
            }
        }
        return SchedulerMessage::Returned(task.result);
    }

    void Scheduler::run(VM& vm) {
        while (true) {
            Task* task = nextTask(vm);
            if (task == nullptr) {
                break; // TODO: do some shit if everything's waiting for something that may be running in a different thread... or don't. i haven't figured out multithreading model yet
            }

            SchedulerMessage message = RunTask(vm, *task);
            switch (message.type) {
                case SchedulerMessageType::Errored:
                    deleteTask(task);
                    break; // TODO: handle it
                case SchedulerMessageType::Called:
                    break; // should never be reached i hope. maybe error if it does
                case SchedulerMessageType::Returned:
                    task->completionFuture->complete(vm, task->result);
                    deleteTask(task);
                    break;
                case SchedulerMessageType::Yielded:
                    enqueue(task);
                    break;
                case SchedulerMessageType::Awaiting:
                    message.future->addWaiter(vm, task);
                    break;
            }
        }
    }

    void Scheduler::deleteTask(Task* task) {
        if (mGCRunning) { // CLion is lying. This condition is NOT always false
            mExpiredTasks.push_back(task);
        } else {
            std::erase(mAllTasks, task);
            delete task;
        }
    }

    uint8_t Scheduler::pickLevel() {
        if (mLevelSelection.empty()) return 0;

        uint8_t start = mLevelCursor;

        do {
            uint8_t level = mLevelSelection[mLevelCursor++];

            if (mLevelCursor >= mLevelSelection.size()) {
                mLevelCursor = 0;
            }

            if (!mReadyQueues[level].empty()) return level;
        } while (mLevelCursor != start);

        return 0;
    }

    Task* Scheduler::nextTask(VM& vm) {
        if (mReadyQueues.empty()) return nullptr;

        if (vm.config().scheduler.priority.levels <= 1) {
            auto& queue = mReadyQueues[0];
            if (queue.empty()) return nullptr;
            Task* task = queue.front();
            queue.pop_front();
            return task;
        }

        uint8_t level = pickLevel();
        auto& queue = mReadyQueues[level];

        if (queue.empty()) {
            for (size_t i = 0; i < mReadyQueues.size(); i++) {
                size_t index = (level + i) % mReadyQueues.size();
                if (!mReadyQueues[index].empty()) {
                    Task* task = mReadyQueues[index].front();
                    mReadyQueues[index].pop_front();
                    return task;
                }
            }
            return nullptr;
        }

        Task* task = queue.front();
        queue.pop_front();
        return task;
    }
}
