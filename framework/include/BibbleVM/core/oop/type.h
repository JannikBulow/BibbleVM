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

            Handle,

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
        size_t primitiveSize; // When storing something of this type as a primitive (object primitives are pointers), this is the size needed

        union {
            Class* instanceClass; // instance
            const Type* baseType; // array
        };

        bool isObjectType() const { return kind >= Instance && kind <= Future; }

        uint16_t getObjectFieldCount() const; // this if fucking ass but i can't include class.h in object.h and i want those specific functions inline in object.h and c++ just sucks fuck c++ fuck c++
    };

    using TypeID = uint32_t;

    BIBBLEVM_EXPORT size_t GetPrimitiveSizeForTypeKind(Type::Kind kind);
}

#endif // BIBBLEVM_CORE_OOP_TYPE_H
