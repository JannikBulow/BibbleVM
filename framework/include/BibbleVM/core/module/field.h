// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_CORE_MODULE_FIELD_H
#define BIBBLEVM_CORE_MODULE_FIELD_H 1

#include "BibbleVM/core/module/const_pool.h"

namespace bibblevm::module {
    struct Field {
        uint32_t typeID;
        ConstantIndex name;
    };
}

#endif // BIBBLEVM_CORE_MODULE_FIELD_H
