// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/compiler/abi.h"
#include "BibbleVM/compiler/trampoline.h"

#include "BibbleVM/core/error.h"

#include "BibbleVM/vm.h"

namespace bibblevm::compiler {
    executor::SchedulerMessage Trampoline(VM& vm, executor::Frame& frame, executor::Task* task) {
        abi::Frame abiFrame{};
        abiFrame.vm = &vm;
        abiFrame.registers = frame.getRegisters();
        abiFrame.frame = &frame;
        abiFrame.task = task;
        abiFrame.code = frame.getFunction().getCompilationUnit()->baseline->mc;

        abi::LeaveRegisters leaveRegisters{};

        abi::bibblevm_enter(&abiFrame, &leaveRegisters);

        switch (static_cast<abi::LeaveReason>(leaveRegisters.reason)) {
            case abi::LeaveReason::Error:
                return executor::SchedulerMessage::Errored(static_cast<uint8_t>(leaveRegisters.exit1), reinterpret_cast<oop::StringObject*>(leaveRegisters.exit2));
            case abi::LeaveReason::Return:
                return executor::SchedulerMessage::Returned({.isObject = leaveRegisters.exit1 != 0, .ul = leaveRegisters.exit2});

            default:
                return executor::SchedulerMessage::Errored(Error::Type::INTERNAL_ERROR, vm.memoryManager().allocateString(vm, "unknown leave reason from compiled function")->asString());
        }
    }
}
