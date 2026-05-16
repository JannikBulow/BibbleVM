// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/core/executor/task.h"

#include "BibbleVM/_compatibility.h"
#include "BibbleVM/vm.h"

namespace bibblevm {
    uint8_t executor::MapPriorityToLevel(VM& vm, TaskPriority priority) {
        uint8_t levels = vm.config().scheduler.priority.levels;
        switch (priority) {
            case TaskPriority::Critical: return 0;
            case TaskPriority::Normal: return levels / 3;
            case TaskPriority::Low: return (2 * levels) / 3;
            case TaskPriority::Background: return levels - 2;
            case TaskPriority::System: return levels - 1;
        }

        BIBBLEVM_UNREACHABLE();
    }
}
