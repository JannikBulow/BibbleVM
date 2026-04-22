// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_CORE_MODULE_CLASS_H
#define BIBBLEVM_CORE_MODULE_CLASS_H 1

#include "BibbleVM/core/module/field.h"
#include "BibbleVM/core/module/method.h"

namespace bibblevm::module {
    struct Class {
        ConstantIndex name;
        ConstantIndex superClass;

        uint16_t fieldCount;
        uint16_t methodCount;

        Field* fields;
        Method* methods;
    };
}

#endif // BIBBLEVM_CORE_MODULE_CLASS_H
