// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_COMPILER_AOT_TRAP_H
#define BIBBLEVM_COMPILER_AOT_TRAP_H

#include "BibbleVM/core/executor/function.h"

namespace bibblevm::compiler {
    executor::SchedulerMessage AOTCompileTrap(VM& vm, executor::Frame& frame, executor::Task* task);
}

#endif //BIBBLEVM_COMPILER_AOT_TRAP_H
