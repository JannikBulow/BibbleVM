// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_CORE_GC_OLD_GEN_HEAP_H
#define BIBBLEVM_CORE_GC_OLD_GEN_HEAP_H 1

#include "BibbleVM/core/oop/object.h"

#include "BibbleVM/util/time_manager.h"

namespace bibblevm::gc {
    class BIBBLEVM_EXPORT OldGenHeap {
    public:
        enum class Phase {
            Idle,
            Mark,
            Sweep
        };

        OldGenHeap() = default;
        ~OldGenHeap();

        bool init(VM& vm);

        size_t getCapacity() const;
        size_t getUsed() const;
        double getLoadFactor() const;

        bool containsObject(const oop::Object* object) const;

        Phase getPhase() const;
        bool isCollecting() const;

        oop::Object* allocate(size_t byteSize);

        void grow(size_t newRegionSize);
        void shrink();

        void startCollection(VM& vm);
        bool stepCollection(VM& vm, TimeManager<>::TimePoint startTime);

        void markObject(oop::Object* object);

    private:

    };
}

#endif // BIBBLEVM_CORE_GC_OLD_GEN_HEAP_H
