// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_VALUE_H
#define BIBBLEVM_VALUE_H 1

#include "BibbleVM/util/string_pool.h"

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
        struct Future;
    }

    union Value {
        Byte b;
        UByte ub;
        Short s;
        UShort us;
        Int i;
        UInt ui;
        Long l;
        ULong ul;
        String str;
        executor::Module* mi;
        executor::Function* fi;
        executor::Future* future;
    };
}

#endif //BIBBLEVM_VALUE_H