// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/allocator/arena.h"

#include <libos/memory.h>

namespace bibblevm {
    std::optional<StaticArenaAllocator> StaticArenaAllocator::Create(size_t size, bool preCommit) {
        size = os_mem_aligntopagesize(size);

        void* base;
        os_result res;

        if (preCommit) {
            res = os_mem_allocate(&base, nullptr, size, OS_MEM_RESERVE | OS_MEM_COMMIT, OS_MEM_PROTECT_READWRITE);
        } else {
            res = os_mem_allocate(&base, nullptr, size, OS_MEM_RESERVE, OS_MEM_PROTECT_NOACCESS);
        }

        if (res != OS_OK) {
            return std::nullopt;
        }

        return StaticArenaAllocator(base, size, preCommit ? size : 0);
    }

    void* StaticArenaAllocator::allocate(size_t size) {
        if (mUsed + size > mSize) return nullptr;

        void* pointer = static_cast<char*>(mBase) + mUsed;

        if (mUsed + size > mCommitted) {
            void* committed;
            size_t committedSize = os_mem_aligntopagesize(size);
            os_result res = os_mem_allocate(&committed, pointer, committedSize, OS_MEM_COMMIT, OS_MEM_PROTECT_READWRITE);
            if (res != OS_OK) {
                return nullptr;
            }
        }

        return pointer;
    }

    void StaticArenaAllocator::clear(bool decommitMemory) {
        mUsed = 0;

        if (decommitMemory) {
            os_result res = os_mem_free(mBase, mCommitted, OS_MEM_DECOMMIT);
            if (res != OS_OK) {
                //TODO: do something here. for now, just set a breakpoint at this variable declaration
                int a = 69;
            } else {
                mCommitted = 0;
            }
        }
    }

    StaticArenaAllocator::StaticArenaAllocator(void* base, size_t size, size_t committed)
        : mBase(base)
        , mSize(size)
        , mCommitted(committed) {}
}
