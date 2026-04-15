// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_CORE_GC_OLD_GEN_HEAP_H
#define BIBBLEVM_CORE_GC_OLD_GEN_HEAP_H 1

#include "BibbleVM/core/oop/object.h"

#include "BibbleVM/core/executor/stack.h"

#include "BibbleVM/util/time_manager.h"

#include <vector>

namespace bibblevm::gc {
    class BIBBLEVM_EXPORT OldGenHeap {
    public:
        enum class Phase {
            Idle,
            Roots,
            Mark,
            Sweep,
            Shrink
        };

        OldGenHeap() = default;
        ~OldGenHeap();

        bool init(VM& vm);

        size_t getCapacity() const { return mTotalCapacity; }
        size_t getUsed() const { return mTotalUsed; };
        double getLoadFactor() const { return static_cast<double>(getUsed()) / static_cast<double>(getCapacity()); };

        Phase getPhase() const { return mPhase; }
        bool isCollecting() const { return mPhase != Phase::Idle; }

        bool containsObject(oop::Object* object);

        oop::Object* allocate(VM& vm, size_t byteSize);

        /*
        USE WITH CAUTION!
        This function could fuck up the running program in an unrecoverable way,
        and it may become private in the future
        */
        void deallocate(VM& vm, oop::Object* object);

        bool grow(VM& vm);
        bool shrink(VM& vm);

        void startCollection(VM& vm);

        void stepCollection(VM& vm, TimeManager<>::TimePoint deadline);

        void markObject(oop::Object* object);

    private:
        struct Region {
            static constexpr uint64_t MAGIC = 0x524547494F4E4845ULL;

            uint64_t magic;

            size_t liveBytes;

            Region* next;

            uint8_t* allocPointer;
            uint8_t* end; // i don't like storing this, but os_mem_free needs a size
            uint8_t start[];

            static Region* Create(size_t size, Region* next);
            void destroy();

            size_t getUsableSize() const { return end - start; }
            size_t getTotalSize() const { return end - reinterpret_cast<const uint8_t*>(this); }

            bool contains(const void* pointer) const;
        };

        struct RootsState {
            size_t stackIndex = 0;
            executor::Frame* currentFrame = nullptr;
            size_t rootIndex = 0;
        };

        struct SweepState {
            Region* currentRegion = nullptr;
            uint8_t* currentSweep = nullptr;
        };

        struct ShrinkState {
            Region* previousRegion = nullptr;
            Region* currentRegion = nullptr;
        };

        Region* mHead = nullptr;
        Region* mAllocRegion = nullptr;

        size_t mTotalCapacity = 0;
        size_t mTotalUsed = 0;

        Phase mPhase = Phase::Idle;
        uint8_t mMarkEpoch = 0;

        union {
            RootsState roots{};
            SweepState sweep;
            ShrinkState shrink;
        } mState;

        std::vector<oop::Object*> mMarkQueue;

        Region* getObjectRegion(VM& vm, oop::Object* object);

        bool grow(size_t regionSize, size_t regionCount);

        bool rootsStep(VM& vm, TimeManager<>::TimePoint deadline);
        bool markStep(VM& vm);
        bool sweepStep(VM& vm);
        bool shrinkStep(VM& vm);

        void scanObject(VM& vm, oop::Object* object);
    };
}

#endif // BIBBLEVM_CORE_GC_OLD_GEN_HEAP_H
