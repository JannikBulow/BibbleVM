// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_CORE_VALUE_H
#define BIBBLEVM_CORE_VALUE_H 1

#include "BibbleVM/core/oop/type.h"

#include <cstdint>

namespace bibblevm {
    using Byte = int8_t;
    using UByte = uint8_t;
    using Short = int16_t;
    using UShort = uint16_t;
    using Int = int32_t;
    using UInt = uint32_t;
    using Long = int64_t;
    using ULong = uint64_t;

    namespace executor {
        class Function;
        class Module;
    }

    namespace oop {
        class Class;
        struct Field;
        struct Method;
        struct Object;
    }

    struct Value {
        bool isObject;
        union {
            Byte b;
            UByte ub;
            Short s;
            UShort us;
            Int i;
            UInt ui;
            Long l;
            ULong ul;
            float f;
            double d;
            oop::Object* obj;
            executor::Module* mi;
            oop::Class* ci;
            oop::Field* fi;
            oop::Method* mei;
            executor::Function* fni;
        };
    };
}

#endif //BIBBLEVM_CORE_VALUE_H