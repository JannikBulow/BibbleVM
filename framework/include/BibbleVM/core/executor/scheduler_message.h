// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_CORE_EXECUTOR_SCHEDULER_MESSAGE_H
#define BIBBLEVM_CORE_EXECUTOR_SCHEDULER_MESSAGE_H 1

#include "BibbleVM/core/executor/task.h"

#include "BibbleVM/util/string_pool.h"

namespace bibblevm::executor {
    enum class SchedulerMessageType {
        Errored,
        Called,
        Returned,
        Yielded,
        Awaiting
    };

    struct SchedulerMessage {
        SchedulerMessageType type;
        union {
            struct { Error::Type type; String message; } error;
            struct { Function* function; uint16_t returnRegister; uint16_t argsBegin; } call;
            Value returnValue;
            oop::Future* future;
        };

        constexpr SchedulerMessage(SchedulerMessageType type) : type(type) {}

        static inline SchedulerMessage Errored(Error::Type type, String message = nullptr) {
            SchedulerMessage m = SchedulerMessageType::Errored;
            m.error.type = type;
            m.error.message = message;
            return m;
        }
        static constexpr SchedulerMessage Called(Function* function, uint16_t returnRegister, uint16_t argsBegin) {
            SchedulerMessage m = SchedulerMessageType::Called;
            m.call.function = function;
            m.call.returnRegister = returnRegister;
            m.call.argsBegin = argsBegin;
            return m;
        }
        static constexpr SchedulerMessage Returned(Value returnValue) {
            SchedulerMessage m = SchedulerMessageType::Returned;
            m.returnValue = returnValue;
            return m;
        }
        static constexpr SchedulerMessage Yielded() { return SchedulerMessageType::Yielded; }
        static constexpr SchedulerMessage Awaiting(oop::Future* future) {
            SchedulerMessage m = SchedulerMessageType::Awaiting;
            m.future = future;
            return m;
        }
    };
}

#endif // BIBBLEVM_CORE_EXECUTOR_SCHEDULER_MESSAGE_H
