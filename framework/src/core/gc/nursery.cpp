// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/core/gc/nursery.h"

#include <libos/memory.h>

#include <ranges>
#include <utility>

namespace bibblevm::gc {
    Nursery::~Nursery() {
        destroy();
    }

    Nursery& Nursery::operator=(Nursery&& other) noexcept {
        if (fromStart == other.fromStart) return *this;

        destroy();

        fromStart = std::exchange(other.fromStart, nullptr);
        fromEnd = std::exchange(other.fromEnd, nullptr);
        fromAllocPointer = std::exchange(other.fromAllocPointer, nullptr);
        toStart = std::exchange(other.toStart, nullptr);
        toEnd = std::exchange(other.toEnd, nullptr);
        toAllocPointer = std::exchange(other.toAllocPointer, nullptr);

        return *this;
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
        fromAllocPointer = fromStart;
        toStart = memory + size;
        toEnd = memory + size * 2;
        toAllocPointer = toStart;

        return true;
    }

    uint8_t* Nursery::getMemoryStart() const {
        return std::min(fromStart, toStart);
    }

    uint8_t* Nursery::getMemoryEnd() const {
        return std::max(fromEnd, toEnd);
    }

    size_t Nursery::getSpaceSize() const {
        return fromEnd - fromStart; // both spaces are same size so we ball
    }

    void Nursery::swap() {
        std::swap(fromStart, toStart);
        std::swap(fromEnd, toEnd);
        std::swap(fromAllocPointer, toAllocPointer);
    }

    oop::Object* Nursery::allocateInFromSpace(size_t byteSize) {
        byteSize = (byteSize + 15) & ~15;
        if (fromAllocPointer + byteSize > fromEnd) return nullptr;

        oop::Object* object = reinterpret_cast<oop::Object*>(fromAllocPointer);
        new(object) oop::Object(0, byteSize, nullptr, 0, 0);
        fromAllocPointer += byteSize;
        return object;
    }

    oop::Object* Nursery::allocateInToSpace(size_t byteSize) {
        byteSize = (byteSize + 15) & ~15;
        if (toAllocPointer + byteSize > toEnd) return nullptr;

        oop::Object* object = reinterpret_cast<oop::Object*>(toAllocPointer);
        new(object) oop::Object(0, byteSize, nullptr, 0, 0);
        toAllocPointer += byteSize;
        return object;
    }

    double Nursery::getFromLoadFactor() const {
        size_t capacity = fromEnd - fromStart;
        size_t used = fromAllocPointer - fromStart;
        return static_cast<double>(used) / static_cast<double>(capacity);
    }

    double Nursery::getToLoadFactor() const {
        size_t capacity = toEnd - toStart;
        size_t used = toAllocPointer - toStart;
        return static_cast<double>(used) / static_cast<double>(capacity);
    }

    void Nursery::resetFromSpace() {
        fromAllocPointer = fromStart;
    }

    void Nursery::resetToSpace() {
        toAllocPointer = toStart;
    }

    bool Nursery::isInFromSpace(const void* pointer) const {
        return fromStart <= pointer && pointer < fromEnd;
    }

    bool Nursery::isInToSpace(const void* pointer) const {
        return toStart <= pointer && pointer < toEnd;
    }

    void Nursery::destroy() {
        if (fromStart == nullptr) return;

        uint8_t* memoryStart = getMemoryStart();
        uint8_t* memoryEnd = getMemoryEnd();
        os_result res = os_mem_free(memoryStart, memoryEnd - memoryStart, OS_MEM_RELEASE);
        if (res != OS_OK) {
            int a = 52; // idk bruh. just breakpoint here while debugging
        }
    }
}
