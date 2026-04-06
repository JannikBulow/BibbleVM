// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_CORE_GC_NURSERY_H
#define BIBBLEVM_CORE_GC_NURSERY_H 1

#include "BibbleVM/core/oop/object.h"

#include "BibbleVM/api.h"

#include <cstdint>

namespace bibblevm::gc {
    // I wish I could use an arena allocator here, but I need some super specific shit for this.
    struct BIBBLEVM_EXPORT Nursery {
        uint8_t* fromStart;
        uint8_t* fromEnd;

        uint8_t* toStart;
        uint8_t* toEnd;

        uint8_t* allocPointer;

        Nursery() = default;
        ~Nursery();

        bool init(size_t size);

        void swap();

        oop::Object* allocate(size_t byteSize);

        bool isInFromSpace(const void* pointer) const;
        bool isInToSpace(const void* pointer) const;
    };
}

#endif // BIBBLEVM_CORE_GC_NURSERY_H
