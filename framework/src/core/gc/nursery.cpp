// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/core/gc/nursery.h"

#include <libos/memory.h>

#include <ranges>

namespace bibblevm::gc {
    Nursery::~Nursery() {
        uint8_t* memoryStart = std::min(fromStart, toStart);
        uint8_t* memoryEnd = std::max(fromEnd, toEnd);
        os_result res = os_mem_free(memoryStart, memoryEnd - memoryStart, OS_MEM_RELEASE);
        if (res != OS_OK) {
            int a = 52; // idk bruh. just breakpoint here while debugging
        }
    }

    bool Nursery::init(size_t size) {
        size = os_mem_aligntopagesize(size); //TODO: detect if large pages are better

        uint8_t* memory;
        os_result res = os_mem_allocate(reinterpret_cast<void**>(&memory), nullptr, size * 2, OS_MEM_RESERVE | OS_MEM_COMMIT | OS_MEM_ZERO, OS_MEM_PROTECT_READWRITE);
        if (res != OS_OK) {
            return false;
        }

        fromStart = memory;
        fromEnd = memory + size;
        toStart = memory + size;
        toEnd = memory + size * 2;
        allocPointer = fromStart;

        return true;
    }

    void Nursery::swap() {
        std::swap(fromStart, toStart);
        std::swap(fromEnd, toEnd);
    }

    oop::Object* Nursery::allocate(size_t byteSize) {
        byteSize = (byteSize + 15) & ~15;
        if (allocPointer + byteSize > fromEnd) return nullptr;

        oop::Object* object = reinterpret_cast<oop::Object*>(allocPointer);
        new(object) oop::Object(0, byteSize, nullptr);
        allocPointer += byteSize;
        return object;
    }

    bool Nursery::isInFromSpace(const void* pointer) const {
        return fromStart <= pointer && pointer < fromEnd;
    }

    bool Nursery::isInToSpace(const void* pointer) const {
        return toStart <= pointer && pointer < toEnd;
    }
}
