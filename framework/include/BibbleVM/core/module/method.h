// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_CORE_MODULE_METHOD_H
#define BIBBLEVM_CORE_MODULE_METHOD_H 1

#include "BibbleVM/core/module/const_pool.h"

namespace bibblevm::module {
    struct Method {
        ConstantIndex name;
        ConstantIndex function;
    };
}

#endif // BIBBLEVM_CORE_MODULE_METHOD_H
