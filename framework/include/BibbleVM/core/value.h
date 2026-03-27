// Copyright 2026 JesusTouchMe

#ifndef BIBBLEVM_VALUE_H
#define BIBBLEVM_VALUE_H 1

#include <cstdint>

namespace bibblevm {
    using Byte = int8_t;
    using Short = int16_t;
    using Int = int32_t;
    using Long = int64_t;

    union Value {
        Byte b;
        Short s;
        Int i;
        Long l;
    };
}

#endif //BIBBLEVM_VALUE_H