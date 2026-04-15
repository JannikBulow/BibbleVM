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
        uint8_t* fromAllocPointer;

        uint8_t* toStart;
        uint8_t* toEnd;
        uint8_t* toAllocPointer;

        Nursery() = default;
        ~Nursery();

        Nursery& operator=(Nursery&& other) noexcept;

        bool init(size_t size);

        uint8_t* getMemoryStart() const;
        uint8_t* getMemoryEnd() const;

        size_t getSpaceSize() const;

        void swap();

        oop::Object* allocateInFromSpace(size_t byteSize);
        oop::Object* allocateInToSpace(size_t byteSize);

        double getFromLoadFactor() const;
        double getToLoadFactor() const;

        void resetFromSpace();
        void resetToSpace();

        bool isInFromSpace(const oop::Object* object) const;
        bool isInToSpace(const oop::Object* object) const;

    private:
        void destroy();
    };
}

#endif // BIBBLEVM_CORE_GC_NURSERY_H
