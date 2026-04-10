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

        void grow(VM& vm);
        void shrink(VM& vm);

        void startCollection(VM& vm);
        bool stepCollection(VM& vm, TimeManager<>::TimePoint startTime);

        void markObject(oop::Object* object);

    private:
        struct Region {
            uint8_t* start;
            uint8_t* allocPointer;
            Region* next;

            static Region* Create(size_t size, Region* next);
            void destroy();
        };

        Region* mHead = nullptr;

        size_t mTotalCapacity = 0;
        size_t mTotalUsed = 0;

        Phase mPhase = Phase::Idle;
        uint8_t mCurrentEpoch : 1 = 0;
    };
}

#endif // BIBBLEVM_CORE_GC_OLD_GEN_HEAP_H
