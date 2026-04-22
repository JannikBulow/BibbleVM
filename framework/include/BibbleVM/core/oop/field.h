// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_CORE_OOP_FIELD_H
#define BIBBLEVM_CORE_OOP_FIELD_H 1

#include "BibbleVM/core/oop/string.h"

#include <cstdint>

namespace bibblevm::oop {
    struct Field {
        Type type;
        uint64_t memoryOffset;
        String name;

        Field(Type type, String name) : type(type), memoryOffset(0), name(name) {}
    };
}

#endif // BIBBLEVM_CORE_OOP_FIELD_H
