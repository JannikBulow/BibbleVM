// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/core/gc/old_gen_heap.h"

#include "BibbleVM/core/oop/class.h"

#include "BibbleVM/_debug.h"
#include "BibbleVM/vm.h"

#include <libos/memory.h>

#include <cmath>

namespace bibblevm::gc {
    OldGenHeap::~OldGenHeap() {
        for (Region* region = mHead; region != nullptr;) {
            Region* tmp = region;
            region = region->next;
            tmp->destroy();
        }
    }

    bool OldGenHeap::init(VM& vm) {
        if (vm.config().memory.oldGenHeapMinSize == 0) return grow(vm.config().memory.oldGenHeapRegionSize, 1); // Never allow a nullptr head as allocate would then have to always check against that
        return grow(vm.config().memory.oldGenHeapRegionSize, std::ceil(vm.config().memory.oldGenHeapMinSize / vm.config().memory.oldGenHeapRegionSize));
    }

    bool OldGenHeap::containsObject(const oop::Object* object) const {
        for (Region* region = mHead; region != nullptr; region = region->next) {
            if (region->contains(object)) return true;
        }
        return false;
    }

    oop::Object* OldGenHeap::allocate(VM& vm, size_t byteSize) {
        byteSize = (byteSize + 15) & ~15;

        auto createObject = [this, byteSize] {
            oop::Object* object = reinterpret_cast<oop::Object*>(mAllocRegion->allocPointer);
            new(object) oop::Object(0, byteSize, nullptr, 0, 0);
            if (isCollecting()) object->markBit = mMarkEpoch; // It's fine to only set the mark bit since sweep will only check for that
            mAllocRegion->allocPointer += byteSize;
            mAllocRegion->liveBytes += byteSize;
            mTotalUsed += byteSize;
            return object;
        };

        // fast path

        if (mAllocRegion->allocPointer + byteSize <= mAllocRegion->end) [[likely]] { // Should this be likely?
            return createObject();
        }

        // slow path. look for a region that can hold this

        Region* region = mAllocRegion->next;
        for (; region != nullptr; region = region->next) {
            if (region->allocPointer + byteSize <= region->end) {
                mAllocRegion = region;
                return createObject();
            }
        }

        if (!grow(vm)) return nullptr;
        mAllocRegion = mHead;

        if (mAllocRegion->allocPointer + byteSize > mAllocRegion->end) return nullptr;

        return createObject();
    }

    void OldGenHeap::deallocate(VM& vm, oop::Object* object) {
        const oop::Type* type = vm.memoryManager().getType(object->type);
        if (type->kind == oop::Type::Instance && type->instanceClass->hasFinalizer()) {
            vm.memoryManager().queueFinalizer(vm, object);
        }

        Region* region = getObjectRegion(vm, object);
        region->liveBytes -= object->allocatedSize;

        mTotalUsed -= object->allocatedSize;
    }

    bool OldGenHeap::grow(VM& vm) {
        if (mTotalCapacity == vm.config().memory.oldGenHeapMaxSize) return false;
        size_t targetSize = std::min(static_cast<size_t>(static_cast<double>(mTotalUsed) * vm.config().memory.oldGenHeapGrowthFactor), vm.config().memory.oldGenHeapMaxSize);
        size_t needed = targetSize - mTotalCapacity;
        size_t regionsNeeded = (needed + vm.config().memory.oldGenHeapRegionSize - 1) / vm.config().memory.oldGenHeapRegionSize;
        return grow(vm.config().memory.oldGenHeapRegionSize, regionsNeeded);
    }

    // This function explicitly destroys regions with 0 live bytes (including ones that were just created with grow()).
    // Only use when severely memory pressured.
    bool OldGenHeap::shrink(VM& vm) {
        // Don't use shrinkStep or mState.shrink because a GC cycle may be active

        Region* prev = nullptr;
        Region* current = mHead;
        while (current != nullptr) {
            if (current->liveBytes == 0) {
                if (prev == nullptr) {
                    BIBBLEVM_ASSERT(current == mHead);

                    mHead = mHead->next;
                    Region* tmp = current;
                    current = current->next;

                    tmp->destroy();
                } else {
                    prev->next = current->next;

                    Region* tmp = current;
                    current = current->next;

                    tmp->destroy();
                }
            } else {
                prev = current;
                current = current->next;
            }
        }

        return true;
    }

    void OldGenHeap::startCollection(VM& vm) {
        if (isCollecting()) return;

        mMarkEpoch ^= 1;
        mMarkQueue.clear();

        mState.roots = {};

        mPhase = Phase::Roots;
    }

    void OldGenHeap::stepCollection(VM& vm, TimeManager<>::TimePoint deadline) {
        while (vm.timeManager().now() < deadline) {
            switch (mPhase) {
                case Phase::Idle:
                    return;
                case Phase::Roots:
                    if (!rootsStep(vm, deadline))
                        mPhase = Phase::Mark;
                    break;
                case Phase::Mark:
                    if (!markStep(vm)) {
                        mPhase = Phase::Sweep;
                        mState.sweep = {};
                        mState.sweep.currentRegion = mHead;
                        mState.sweep.currentSweep = mHead->start;
                    }
                    break;
                case Phase::Sweep:
                    if (!sweepStep(vm)) {
                        mPhase = Phase::Shrink;
                        mState.shrink = {};
                        mState.shrink.currentRegion = mHead;
                    }
                    break;

                case Phase::Shrink:
                    if (!shrinkStep(vm))
                        mPhase = Phase::Idle;
                    break;
            }
        }
    }

    void OldGenHeap::markObject(oop::Object* object) {
        if (object == nullptr) return;
        if (!containsObject(object)) return;
        if (object->markBit == mMarkEpoch) return;

        object->markBit = mMarkEpoch;
        mMarkQueue.push_back(object);
    }

    OldGenHeap::Region* OldGenHeap::Region::Create(size_t size, Region* next) {
        size = os_mem_aligntopagesize(size);

        void* memory;
        os_result res = os_mem_allocate(&memory, nullptr, size, OS_MEM_RESERVE | OS_MEM_COMMIT | OS_MEM_ZERO, OS_MEM_PROTECT_READWRITE);
        if (res != OS_OK) return nullptr;

        Region* region = static_cast<Region*>(memory);

        region->magic = MAGIC;
        region->next = next;
        region->allocPointer = region->start;
        region->end = static_cast<uint8_t*>(memory) + size;

        return region;
    }

    void OldGenHeap::Region::destroy() {
        os_result res = os_mem_free(this, getTotalSize(), OS_MEM_RELEASE);
        if (res != OS_OK) {
            int a = 55; // TODO: i'm gonna have fun doing error management later
        }
    }

    bool OldGenHeap::Region::contains(const void* pointer) const {
        return start <= pointer && pointer < end;
    }

    OldGenHeap::Region* OldGenHeap::getObjectRegion(VM& vm, oop::Object* object) {
        Region* region = reinterpret_cast<Region*>(
            reinterpret_cast<uintptr_t>(object) & ~(vm.config().memory.oldGenHeapRegionSize - 1)
        );

        BIBBLEVM_ASSERT(region->magic == Region::MAGIC);
        BIBBLEVM_ASSERT(reinterpret_cast<uint8_t*>(object) >= region->start);
        BIBBLEVM_ASSERT(reinterpret_cast<uint8_t*>(object) < region->end);

        return region;
    }

    bool OldGenHeap::grow(size_t regionSize, size_t regionCount) {
        for (size_t i = 0; i < regionCount; i++) {
            Region* region = Region::Create(regionSize, mHead);
            if (region == nullptr) return false;

            mHead = region;

            mTotalCapacity += region->getUsableSize();
        }
        mAllocRegion = mHead;
        return true;
    }

    bool OldGenHeap::rootsStep(VM& vm, TimeManager<>::TimePoint deadline) {
        auto& state = mState.roots;

        auto& allTasks = vm.scheduler().allTasks();
        auto& roots = vm.memoryManager().roots();

        size_t stackCount = allTasks.size();
        size_t rootCount = roots.size();

        for (; state.stackIndex < stackCount; state.stackIndex++) {
            if (vm.timeManager().now() >= deadline) return true;

            //TODO: make this safer since the program could tear itself down and then this would track nonexistent frame pointers
            if (state.currentFrame == nullptr) state.currentFrame = allTasks[state.stackIndex]->stack.getTop();
            for (; state.currentFrame != nullptr; state.currentFrame = state.currentFrame->getPrev()) {
                if (vm.timeManager().now() >= deadline) return true;

                executor::Frame& frame = *state.currentFrame;
                uint16_t registerCount = frame.getFunction().getRegisterCount();
                for (uint16_t i = 0; i < registerCount; i++) {
                    if (vm.memoryManager().getType(frame[i].type)->isObjectType()) {
                        markObject(frame[i].obj);
                    }
                }
            }
        }

        for (; state.rootIndex < rootCount; state.rootIndex++) {
            if (vm.timeManager().now() >= deadline) return true;

            for (oop::Object*& object : roots[state.rootIndex]) {
                markObject(object);
            }
        }

        return false;
    }

    bool OldGenHeap::markStep(VM& vm) {
        if (mMarkQueue.empty()) return false;

        oop::Object* object = mMarkQueue.back();
        mMarkQueue.pop_back();

        scanObject(vm, object);

        return true;
    }

    bool OldGenHeap::sweepStep(VM& vm) {
        auto& state = mState.sweep;

        if (state.currentRegion == nullptr) return false;

        if (state.currentSweep >= state.currentRegion->allocPointer) {
            state.currentRegion = state.currentRegion->next;
            state.currentSweep = state.currentRegion->start;
            return true;
        }

        oop::Object* object = reinterpret_cast<oop::Object*>(state.currentSweep);
        state.currentSweep += object->allocatedSize;

        if (object->markBit != mMarkEpoch) {
            deallocate(vm, object);
        }

        return true;
    }

    bool OldGenHeap::shrinkStep(VM& vm) {
        auto& state = mState.shrink;

        if (state.currentRegion == nullptr) return false;

        if (state.currentRegion->liveBytes == 0) {
            state.currentRegion->allocPointer = state.currentRegion->start;
            state.previousRegion = state.currentRegion;
            state.currentRegion = state.currentRegion->next;

            // Below is logic for destroying and unlinking the region. For now we will just reset the alloc pointer to reuse all regions
            // TODO: determine when to shrink and when to reuse
            /*
            if (mAllocRegion == state.currentRegion) mAllocRegion = mAllocRegion->next;

            if (state.previousRegion == nullptr) { // current region is head
                BIBBLEVM_ASSERT(state.currentRegion == mHead);

                mHead = mHead->next;
                Region* tmp = state.currentRegion;
                state.currentRegion = state.currentRegion->next;

                tmp->destroy();
            } else {
                state.previousRegion->next = state.currentRegion->next;
                Region* tmp = state.currentRegion;
                state.currentRegion = state.currentRegion->next;

                tmp->destroy();
            }
            */
        } else {
            state.previousRegion = state.currentRegion;
            state.currentRegion = state.currentRegion->next;
        }

        return true;
    }

    void OldGenHeap::scanObject(VM& vm, oop::Object* object) {
        object->visitChildren(vm.memoryManager().getType(object->type), [this](oop::Object*& child) {
            markObject(child);
        });
    }
}
