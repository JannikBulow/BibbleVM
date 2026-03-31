// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/executor/scheduler.h"

namespace bibblevm::executor {
    void Scheduler::enqueue(Task* task) {
        mReadyQueue.push_back(task);
    }

    Task* Scheduler::schedule(Function& function) {
        Task* task = new Task();

        task->scheduler = this;

        task->stack.pushFrame(function);

        //TODO: make Future be GC allocated once GC exists. for now, just leak futures as they're small
        Future* future = new Future();
        task->completionFuture = future;

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

                    break;
                }
                case SchedulerMessageType::Returned: {
                    task.result = message.returnValue;
                    task.stack.popFrame();
                    if (task.stack.getTop() != nullptr) (*task.stack.getTop())[0] = message.returnValue;
                    break;
                }
                case SchedulerMessageType::Yielded: return message;
                case SchedulerMessageType::Awaiting: return message;
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
                    task->completionFuture->complete(task->result);
                    delete task;
                    break;
                case SchedulerMessageType::Yielded:
                    mReadyQueue.push_back(task);
                    break;
                case SchedulerMessageType::Awaiting:
                    message.future->waiters.push_back(task);
                    break;
            }
        }
    }
}
