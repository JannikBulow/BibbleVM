// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_ALLOCATOR_ARENA_H
#define BIBBLEVM_ALLOCATOR_ARENA_H 1

#include <optional>

namespace bibblevm {
    // Arena allocator which remains a static size.
    class StaticArenaAllocator {
    public:
        static std::optional<StaticArenaAllocator> Create(size_t size, bool preCommit = false);

        void* allocate(size_t size);
        void clear(bool decommitMemory = false);

    private:
        void* mBase = nullptr;
        size_t mSize = 0;
        size_t mCommitted = 0;
        size_t mUsed = 0;

        StaticArenaAllocator(void* base, size_t size, size_t committed);
    };
}

#endif // BIBBLEVM_ALLOCATOR_ARENA_H
