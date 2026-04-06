// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_CORE_OOP_METHOD_H
#define BIBBLEVM_CORE_OOP_METHOD_H 1

#include "BibbleVM/core/executor/function.h"

namespace bibblevm::oop {
    struct Method {
        String name;
        uint32_t vtableIndex;
        executor::Function* function;

        Method(String name, uint32_t vtableIndex, executor::Function* function) : name(name), vtableIndex(vtableIndex), function(function) {}
    };
}

#endif // BIBBLEVM_CORE_OOP_METHOD_H
