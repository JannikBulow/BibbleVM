// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/core/executor/scheduler.h"

#include "BibbleVM/vm.h"

#include <algorithm>

namespace bibblevm::executor {
    void Scheduler::safeDeleteExpiredTasks() {
        for (Task* task : mExpiredTasks) {
            delete task;
        }
        mExpiredTasks.clear();
    }

    void Scheduler::enqueue(Task* task) {
        mReadyQueue.push_back(task);
    }

    Task* Scheduler::schedule(VM& vm, Function& function, Value* arguments) {
        Task* task = new Task();

        task->scheduler = this;

        Frame& frame = task->stack.pushFrame(function);
        memcpy(&frame[0], arguments, function.getParameterCount() * sizeof(Value));

        oop::Object* futureObject = vm.memoryManager().allocateFuture();
        task->completionFuture = futureObject->asFuture();

        mAllTasks.push_back(task);
        mReadyQueue.push_back(task);

        return task;
    }

    static SchedulerMessage RunTask(VM& vm, Task& task) {
        while (task.stack.getTop() != nullptr) {
            Frame& frame = *task.stack.getTop();

            SchedulerMessage message = frame.getFunction().invoke(vm, frame);
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
        while (!mReadyQueue.empty()) {
            Task* task = mReadyQueue.front();
            mReadyQueue.pop_front();

            SchedulerMessage message = RunTask(vm, *task);
            switch (message.type) {
                case SchedulerMessageType::Errored:
                    break; // TODO: handle it
                case SchedulerMessageType::Called:
                    break; // should never be reached i hope. maybe error if it does
                case SchedulerMessageType::Returned:
                    task->completionFuture->complete(vm, task->result);
                    deleteTask(task);
                    break;
                case SchedulerMessageType::Yielded:
                    mReadyQueue.push_back(task);
                    break;
                case SchedulerMessageType::Awaiting:
                    message.future->addWaiter(vm, task);
                    break;
            }
        }
    }

    void Scheduler::deleteTask(Task* task) {
        if (mGCRunning) {
            mExpiredTasks.push_back(task);
        } else {
            std::erase(mAllTasks, task);
            delete task;
        }
    }
}
