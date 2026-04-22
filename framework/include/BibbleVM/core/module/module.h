// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_CORE_MODULE_MODULE_H
#define BIBBLEVM_CORE_MODULE_MODULE_H 1

#include "BibbleVM/core/module/class.h"
#include "BibbleVM/core/module/function.h"

namespace bibblevm::module {
    constexpr uint32_t MAGIC = 0xFAAC6767;

    struct Module {
        uint32_t magic;
        uint16_t formatVersion;

        ConstantIndex name;

        ConstPool constPool;

        uint16_t classCount;
        uint16_t functionCount;

        Class* classes;
        Function* functions;
    };
}

#endif // BIBBLEVM_CORE_MODULE_MODULE_H
