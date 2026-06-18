// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/compiler/abi.h"
#include "BibbleVM/compiler/trampoline.h"

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

        return executor::SchedulerMessage::Yielded();
    }
}
