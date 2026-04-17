// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_CORE_OOP_TYPE_H
#define BIBBLEVM_CORE_OOP_TYPE_H 1

#include "BibbleVM/api.h"

#include <cstdint>

namespace bibblevm::gc {
    class MemoryManager;
}

namespace bibblevm::oop {
    enum class Type : uint8_t {
        // Primitives
        Byte,
        UByte,
        Short,
        UShort,
        Int,
        UInt,
        Long,
        ULong,
        Float,
        Double,

        Handle,

        Reference, // instance, array, string, future

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
