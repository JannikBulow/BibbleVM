// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_COMPILER_TRAMPOLINE_H
#define BIBBLEVM_COMPILER_TRAMPOLINE_H 1

#include "BibbleVM/core/executor/scheduler_message.h"
#include "BibbleVM/core/executor/stack.h"

namespace bibblevm::compiler {
    BIBBLEVM_EXPORT executor::SchedulerMessage Trampoline(VM& vm, executor::Frame& frame, executor::Task* task);
}

#endif // BIBBLEVM_COMPILER_TRAMPOLINE_H
