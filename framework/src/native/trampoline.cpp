// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/native/trampoline.h"

#include "BibbleVM/vm.h"

namespace bibblevm::native {
    using Signature = VMReturnValue(BIBBLE_CALL*)(BibbleVM*, BibbleInterface*, VMValue*);

    executor::SchedulerMessage FunctionTrampoline(VM& vm, executor::Frame& frame, executor::Task* task) {
        //TODO: allow native functions to run async

        uint16_t parameterCount = frame.getFunction().getParameterCount();
        VMValue* arguments = frame.arena()->allocate<VMValue>(parameterCount);

        vm.memoryManager().pushLocalReferenceFrame(parameterCount);

        for (uint16_t i = 0; i < parameterCount; ++i) {
            arguments[i] = frame[i].toNative(vm.memoryManager());
        }

        void* implementation = frame.getFunction().implementation();
        VMReturnValue ret = reinterpret_cast<Signature>(implementation)(vm, &task->nativeInterface, arguments);
        Value value = Value::FromNative(ret.value, ret.reference);

        vm.memoryManager().popLocalReferenceFrame();

        return executor::SchedulerMessage::Returned(value);
    }
}
