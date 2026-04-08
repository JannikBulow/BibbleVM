// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_CORE_GC_OLD_GEN_HEAP_H
#define BIBBLEVM_CORE_GC_OLD_GEN_HEAP_H

#include "BibbleVM/core/oop/object.h"

namespace bibblevm::gc {
    class OldGenHeap {
    public:
        OldGenHeap() = default;
        ~OldGenHeap();

        bool init(size_t initialSize);

        oop::Object* allocate(size_t byteSize);

        void grow(size_t newRegionSize);
        void shrink();
    };
}

#endif //BIBBLEVM_CORE_GC_OLD_GEN_HEAP_H
