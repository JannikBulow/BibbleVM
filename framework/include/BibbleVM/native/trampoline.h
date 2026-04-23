// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_NATIVE_TRAMPOLINE_H
#define BIBBLEVM_NATIVE_TRAMPOLINE_H 1

#include "BibbleVM/core/executor/function.h"

namespace bibblevm::native {
    BIBBLEVM_EXPORT executor::SchedulerMessage FunctionTrampoline(VM& vm, executor::Frame& frame, executor::Task* task);
}

#endif // BIBBLEVM_NATIVE_TRAMPOLINE_H
