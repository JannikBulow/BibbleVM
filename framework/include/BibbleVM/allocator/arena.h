// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_ALLOCATOR_ARENA_H
#define BIBBLEVM_ALLOCATOR_ARENA_H 1

#include "BibbleVM/api.h"

namespace bibblevm {
    // Arena allocator which remains a static size.
    class BIBBLEVM_EXPORT StaticArenaAllocator {
    public:
        static StaticArenaAllocator* Create(size_t size, bool preCommit = false);

        void release();

        void* allocate(size_t size);
        void clear(bool decommitMemory = false);

    protected:
        size_t mSize = 0;
        size_t mCommitted = 0;
        size_t mUsed = 0;

        StaticArenaAllocator(size_t size, size_t committed);

        void* base();
    };

    class BIBBLEVM_EXPORT ArenaAllocator {
    public:
        ArenaAllocator(StaticArenaAllocator* allocator)
            : mAllocator(allocator) {}

        ~ArenaAllocator() {
            mAllocator->release();
        }

        StaticArenaAllocator* operator->() const {
            return mAllocator;
        }

    private:
        StaticArenaAllocator* mAllocator;
    };

    class BIBBLEVM_EXPORT GrowingArenaAllocator {
    public:
        ~GrowingArenaAllocator();

        static GrowingArenaAllocator Create(size_t minRegionSize, size_t initialSize, bool preCommit = false);

        void* allocate(size_t size);
        void clear(bool decommitMemory = false);

    private:
        struct Region : StaticArenaAllocator {
            Region* prev;

            Region(size_t size, size_t committed, Region* prev)
                : StaticArenaAllocator(size, committed), prev(prev) {}

            static Region* Create(size_t size, Region* prev, bool preCommit);

            void clear(bool decommitMemory = false);
        };

        size_t mMinRegionSize = 0;
        bool mPreCommit = false;
        Region* mHead = nullptr;

        GrowingArenaAllocator(size_t minRegionSize, Region* head, bool preCommit);
    };
}

#endif // BIBBLEVM_ALLOCATOR_ARENA_H
