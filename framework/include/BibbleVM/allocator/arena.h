// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_ALLOCATOR_ARENA_H
#define BIBBLEVM_ALLOCATOR_ARENA_H 1

#include "BibbleVM/api.h"

#include <cstddef>
#include <type_traits>

//TODO: make the arena allocators nicer
namespace bibblevm {
    // Arena allocator which remains a static size.
    class BIBBLEVM_EXPORT StaticArenaAllocator {
    public:
        struct RestorePoint {
            size_t used;
        };

        static StaticArenaAllocator* Create(size_t size, bool preCommit = false);

        void release();

        RestorePoint getRestorePoint() const;
        void restore(RestorePoint point);

        void* allocate(size_t size);
        void clear(bool decommitMemory = false);

    protected:
        size_t mSize = 0;
        size_t mCommitted = 0;
        size_t mUsed = 0;

        StaticArenaAllocator(size_t size, size_t committed);

        void* base();
    };

    // RAII wrapper for StaticArenaAllocator
    class BIBBLEVM_EXPORT ArenaAllocator {
    public:
        using RestorePoint = StaticArenaAllocator::RestorePoint;

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
        struct Region : StaticArenaAllocator {
            Region* prev;

            Region(size_t size, size_t committed, Region* prev)
                : StaticArenaAllocator(size, committed)
                , prev(prev) {}

            static Region* Create(size_t size, Region* prev, bool preCommit);

            size_t& used() { return mUsed; }

            void clear(bool decommitMemory = false);
        };
    public:
        struct RestorePoint {
            Region* head;
            size_t used;
        };

        ~GrowingArenaAllocator();

        static GrowingArenaAllocator Create(size_t minRegionSize, size_t initialSize = 0, bool preCommit = false);

        RestorePoint getRestorePoint() const;
        void restore(RestorePoint point);

        template<class T = void>
        T* allocate(size_t count) {
            if constexpr (std::is_same_v<T, void>) {
                return allocate0(count);
            } else {
                return static_cast<T*>(allocate0(count * sizeof(T)));
            }
        }

        void clear(bool decommitMemory = false);

    private:
        size_t mMinRegionSize = 0;
        bool mPreCommit = false;
        Region* mHead = nullptr;

        GrowingArenaAllocator(size_t minRegionSize, Region* head, bool preCommit);

        void* allocate0(size_t size);
    };

    template<class T>
    class Snapshot {
    public:
        Snapshot(T& arena)
            : mArena(arena)
            , mRestorePoint(arena.getRestorePoint())
            , mMoved(false) {}

        ~Snapshot() {
            if (!mMoved) mArena.restore(mRestorePoint);
        }

        Snapshot(const Snapshot& other) = delete;
        Snapshot& operator=(const Snapshot& other) = delete;

        Snapshot(Snapshot&& other) noexcept
            : mArena(other.mArena)
            , mRestorePoint(other.mRestorePoint)
            , mMoved(other.mMoved) {
            other.mMoved = true;
        }

        T* operator->() const { return &mArena; }

    private:
        T& mArena;
        typename T::RestorePoint mRestorePoint;
        bool mMoved;
    };
}

#endif // BIBBLEVM_ALLOCATOR_ARENA_H
