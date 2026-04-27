// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/allocator/arena.h"

#include <libos/memory.h>

#include <algorithm>
#include <new>

namespace bibblevm {
    // Assume size is aligned to pagesize
    static void* CreateMemory(size_t size, bool preCommit) {
        void* base;
        os_result res;

        if (preCommit) {
            res = os_mem_allocate(&base, nullptr, size, OS_MEM_RESERVE | OS_MEM_COMMIT, OS_MEM_PROTECT_READWRITE);
        } else {
            res = os_mem_allocate(&base, nullptr, size, OS_MEM_RESERVE, OS_MEM_PROTECT_NOACCESS);
            if (res != OS_OK) {
                return nullptr;
            }
            res = os_mem_allocate(&base, base, os_mem_getpagesize(), OS_MEM_COMMIT, OS_MEM_PROTECT_READWRITE);
        }

        if (res != OS_OK) {
            return nullptr;
        }

        return base;
    }

    StaticArenaAllocator* StaticArenaAllocator::Create(size_t size, bool preCommit) {
        size = os_mem_aligntopagesize(size);

        void* base = CreateMemory(size, preCommit);

        if (base == nullptr) {
            return nullptr;
        }

        auto* allocator = new(base) StaticArenaAllocator(size, preCommit ? size : os_mem_getpagesize());
        allocator->allocate(sizeof(StaticArenaAllocator));

        return allocator;
    }

    void StaticArenaAllocator::release() {
        os_mem_free(base(), mSize, OS_MEM_RELEASE);
    }

    StaticArenaAllocator::RestorePoint StaticArenaAllocator::getRestorePoint() const {
        return {mUsed};
    }

    void StaticArenaAllocator::restore(RestorePoint point) {
        mUsed = point.used;
    }

    void* StaticArenaAllocator::allocate(size_t size) {
        size = (size + 15) & ~15;
        if (mUsed + size > mSize) return nullptr;

        void* pointer = static_cast<char*>(base()) + mUsed;

        if (mUsed + size > mCommitted) {
            void* committed;
            size_t committedSize = os_mem_aligntopagesize(size * 8);
            if (committedSize > mSize - mCommitted) committedSize = mSize - mCommitted;
            os_result res = os_mem_allocate(&committed, pointer, committedSize, OS_MEM_COMMIT, OS_MEM_PROTECT_READWRITE);
            if (res != OS_OK) {
                return nullptr;
            }
        }

        mUsed += size;

        return pointer;
    }

    void StaticArenaAllocator::clear(bool decommitMemory) {
        mUsed = sizeof(StaticArenaAllocator);

        if (decommitMemory) [[unlikely]] {
            os_result res = os_mem_free(
                static_cast<char*>(base()) + sizeof(StaticArenaAllocator),
                mCommitted - sizeof(StaticArenaAllocator),
                OS_MEM_DECOMMIT
            );
            if (res != OS_OK) {
                //TODO: do something here. for now, just set a breakpoint at this variable declaration
                int a = 69;
            } else {
                mCommitted = 0;
            }
        }
    }

    StaticArenaAllocator::StaticArenaAllocator(size_t size, size_t committed)
        : mSize(size)
        , mCommitted(committed) {}

    void* StaticArenaAllocator::base() {
        return this;
    }

    // Still leaves the other in a usable state just without owning its previous memory, but C++ still says that it's dead.
    GrowingArenaAllocator::GrowingArenaAllocator(GrowingArenaAllocator&& other) noexcept
        : mMinRegionSize(other.mMinRegionSize)
        , mPreCommit(other.mPreCommit)
        , mHead(other.mHead) {
        other.mHead = nullptr;
    }

    GrowingArenaAllocator::~GrowingArenaAllocator() {
        while (mHead != nullptr) {
            Region* prev = mHead->prev;
            mHead->release();
            mHead = prev;
        }
    }

    GrowingArenaAllocator GrowingArenaAllocator::Create(size_t minRegionSize, size_t initialSize, bool preCommit) {
        if (initialSize == 0) initialSize = minRegionSize;

        minRegionSize = os_mem_aligntopagesize(minRegionSize);
        initialSize = os_mem_aligntopagesize(initialSize);

        Region* region = Region::Create(initialSize, nullptr, preCommit);
        return {minRegionSize, region, preCommit};
    }

    GrowingArenaAllocator::RestorePoint GrowingArenaAllocator::getRestorePoint() const {
        if (mHead == nullptr) return {nullptr, 0};
        return {mHead, mHead->used()};
    }

    void GrowingArenaAllocator::restore(RestorePoint point) {
        while (mHead != nullptr && mHead != point.head) {
            Region* prev = mHead->prev;
            mHead->release();
            mHead = prev;
        }

        if (mHead != nullptr) {
            mHead->used() = point.used;
        }
    }

    void GrowingArenaAllocator::clear(bool decommitMemory) {
        while (mHead->prev != nullptr) {
            Region* prev = mHead->prev;
            mHead->release();
            mHead = prev;
        }
    }

    GrowingArenaAllocator::Region* GrowingArenaAllocator::Region::Create(size_t size, Region* prev, bool preCommit) {
        size = os_mem_aligntopagesize(size);

        void* base = CreateMemory(size, preCommit);

        if (base == nullptr) {
            return nullptr;
        }

        auto* allocator = new(base) Region(size, preCommit ? size : os_mem_getpagesize(), prev);
        allocator->allocate(sizeof(Region));

        return allocator;
    }

    void GrowingArenaAllocator::Region::clear(bool decommitMemory) {
        mUsed = sizeof(Region);

        if (decommitMemory) {
            os_result res = os_mem_free(
                static_cast<char*>(base()) + sizeof(Region),
                mCommitted + sizeof(Region),
                OS_MEM_DECOMMIT
            );
            if (res != OS_OK) {
                //TODO: do something here. for now, just set a breakpoint at this variable declaration
                int a = 69;
            } else {
                mCommitted = 0;
            }
        }
    }

    GrowingArenaAllocator::GrowingArenaAllocator(size_t minRegionSize, Region* head, bool preCommit)
        : mMinRegionSize(minRegionSize)
        , mPreCommit(preCommit)
        , mHead(head) {}

    void* GrowingArenaAllocator::allocate0(size_t size) {
        void* pointer = nullptr;
        if (mHead != nullptr) {
            pointer = mHead->allocate(size);
        }

        if (pointer == nullptr) {
            mHead = Region::Create(os_mem_aligntopagesize(std::max(size, mMinRegionSize)), mHead, mPreCommit);
            if (mHead != nullptr) {
                pointer = mHead->allocate(size);
            }
        }

        return pointer;
    }
}
