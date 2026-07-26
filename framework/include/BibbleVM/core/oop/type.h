// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_CORE_OOP_TYPE_H
#define BIBBLEVM_CORE_OOP_TYPE_H 1

#include "BibbleVM/api.h"

#include <cstddef>
#include <cstdint>

namespace bibblevm::gc {
    class MemoryManager;
}

namespace bibblevm::oop {
    enum class Type : uint8_t {
        // Primitives
        Byte = 0,
        UByte = 1,
        Short = 2,
        UShort = 3,
        Int = 4,
        UInt = 5,
        Long = 6,
        ULong = 7,
        Float = 8,
        Double = 9,

        Handle = 10,

        Reference = 11, // instance, array, string, future

        // Special VM types
        ModuleRef,
        ClassRef,
        FieldRef,
        MethodRef,
        FunctionRef,

        // Not a type kind.
        Count
    };

    BIBBLEVM_EXPORT size_t GetPrimitiveSizeForType(Type type);
}

#endif // BIBBLEVM_CORE_OOP_TYPE_H
