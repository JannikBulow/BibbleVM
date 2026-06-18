// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_CORE_ERROR_H
#define BIBBLEVM_CORE_ERROR_H 1

#include "BibbleVM/core/executor/stack.h"

#include "BibbleVM/util/string_pool.h"

namespace bibblevm {
    struct Error {
        enum Type : uint8_t {
            USERLAND,
            INTERNAL_ERROR, // unrecoverable and usually only caused by bugs
            NULL_REFERENCE,
            INVALID_STATE,
            INDEX_OUT_OF_BOUNDS,
            INVALID_OBJECT_KIND,
            FIELD_RESOLUTION,
            METHOD_RESOLUTION,
        };

        Type type;
        String message;

        executor::Stack stackTrace; // stack of a dead task is moved here

        static std::string_view TypeToString(Type type);

        String describe(VM& vm) const;
    };
}

#endif //BIBBLEVM_CORE_ERROR_H