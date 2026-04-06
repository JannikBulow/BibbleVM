// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_CORE_OOP_TYPE_H
#define BIBBLEVM_CORE_OOP_TYPE_H 1

#include "BibbleVM/api.h"

#include <cstdint>

namespace bibblevm::gc {
    class MemoryManager;
}

namespace bibblevm::oop {
    class Class;
    struct Object;

    struct BIBBLEVM_EXPORT Type {
        enum Kind : uint8_t {
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

            // Objects
            Instance,
            Array,
            String,
            Future,

            // Special VM types
            ModuleRef,
            FunctionRef,

            // Not a type kind.
            Count
        };

        Kind kind;

        union {
            Class* instanceClass; // instance
            const Type* baseType; // array
        };

        bool isObjectType() const { return kind >= Instance && kind <= Future; }

        uint16_t getObjectFieldCount() const; // this if fucking ass but i can't include class.h in object.h and i want those specific functions inline in object.h and c++ just sucks fuck c++ fuck c++
    };

    using TypeID = uint32_t;
}

#endif // BIBBLEVM_CORE_OOP_TYPE_H
