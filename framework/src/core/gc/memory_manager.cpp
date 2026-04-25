// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/core/gc/memory_manager.h"

#include "BibbleVM/core/oop/class.h"
#include "BibbleVM/core/oop/visit_children.h"

#include "BibbleVM/_debug.h"
#include "BibbleVM/vm.h"

namespace bibblevm::gc {
    MemoryManager::MemoryManager()
        : mImmortalAllocator(GrowingArenaAllocator::Create(4 * 1024 * 1024)) {}

    bool MemoryManager::init(VM& vm) {
        mRememberedSet.reserve(vm.config().gc.rememberedSetReserve);
        if (!mOldGenHeap.init(vm)) return false;
        return mNursery.init(vm.config().memory.nurseryMinSize);
    }

    oop::Object* MemoryManager::allocateInstance(VM& vm, oop::Class* clas) {
        oop::Object* object = allocateRawObject(vm, clas->getTotalSize());
        if (object != nullptr) {
            object->asInstance()->clas = clas;
        }
        return object;
    }

    oop::Object* MemoryManager::allocateArray(VM& vm, oop::Type baseType, ULong length) {
        size_t baseTypeSize = oop::GetPrimitiveSizeForType(baseType);

        oop::Object* object = allocateRawObject(vm, sizeof(oop::Array) + baseTypeSize * length);
        if (object != nullptr) {
            object->asArray()->baseType = baseType;
            object->asArray()->length = length;
        }

        return object;
    }

    oop::Object* MemoryManager::reallocateArray(VM& vm, oop::Object* array, ULong newLength) {
        if (array == nullptr) return nullptr;

        oop::Object* newArrayObject = allocateArray(vm, array->asArray()->baseType, newLength);
        if (newArrayObject == nullptr) return nullptr;

        oop::Array* newArray = newArrayObject->asArray();

        memcpy(newArray->elementBytes, array->asArray()->elementBytes, newLength * oop::GetPrimitiveSizeForType(newArray->baseType));

        return newArrayObject;
    }

    oop::Object* MemoryManager::allocateString(VM& vm, ULong lengthBytes) {
        oop::Object* object = allocateRawObject(vm, sizeof(oop::StringObject) + lengthBytes + 1);
        if (object != nullptr) {
            object->asString()->lengthBytes = lengthBytes;
            object->asString()->bytes[lengthBytes] = '\0';
        }
        return object;
    }

    oop::Object* MemoryManager::allocateString(VM& vm, std::string_view copy) {
        oop::Object* object = allocateRawObject(vm, sizeof(oop::StringObject) + copy.size() + 1);
        if (object != nullptr) {
            object->asString()->lengthBytes = copy.size();
            memcpy(object->asString()->bytes, copy.data(), copy.size());
            object->asString()->bytes[copy.size()] = '\0';
        }
        return object;
    }

    oop::Object* MemoryManager::allocateImmortalString(VM& vm, ULong lengthBytes) {
        oop::StringObject* object = static_cast<oop::StringObject*>(mImmortalAllocator.allocate(sizeof(oop::StringObject) + lengthBytes + 1));
        new(object) oop::StringObject();
        object->asObject()->heapID = IMMORTAL_ID;
        object->lengthBytes = lengthBytes;
        object->bytes[lengthBytes] = '\0';
        return object->asObject();
    }

    oop::Object* MemoryManager::allocateImmortalString(VM& vm, std::string_view copy) {
        oop::StringObject* object = static_cast<oop::StringObject*>(mImmortalAllocator.allocate(sizeof(oop::StringObject) + copy.size() + 1));
        new(object) oop::StringObject();
        object->asObject()->heapID = IMMORTAL_ID;
        object->lengthBytes = copy.size();
        memcpy(object->bytes, copy.data(), copy.size());
        object->bytes[copy.size()] = '\0';
        return object->asObject();
    }

    oop::Object* MemoryManager::allocateFuture(VM& vm) {
        oop::Object* object = allocateRawObject(vm, sizeof(oop::Future));
        if (object != nullptr) {
            object->asFuture()->ready = false;
            object->asFuture()->waiters = allocateArray(vm, oop::Type::Handle, 0);
            object->asFuture()->waiterCount = 0;
        }
        return object;
    }

    void MemoryManager::writeBarrier(oop::Object* object, oop::Object* child) {
        if (mOldGenHeap.containsObject(object) && mNursery.isInFromSpace(child)) {
            mRememberedSet.push_back(object);
        }

        if (mOldGenHeap.isCollecting()) {
            mOldGenHeap.markObject(child);
        }
    }

    oop::Object** MemoryManager::newGlobalStrongReference(oop::Object* object) {
        oop::Object** reference = new oop::Object*;
        *reference = object;
        mStrongReferences.insert(reference);
        return reference;
    }

    void MemoryManager::deleteGlobalStrongReference(oop::Object** reference) {
        mStrongReferences.erase(reference);
    }

    void MemoryManager::pushLocalReferenceFrame(size_t initialSize) {
        mLocalReferenceStack.emplace_back();
        mLocalReferenceStack.back().references.reserve(initialSize);
    }

    void MemoryManager::popLocalReferenceFrame() {
        for (oop::Object** reference : mLocalReferenceStack.back().references) {
            deleteGlobalStrongReference(reference);
        }
    }

    oop::Object** MemoryManager::newLocalStrongReference(oop::Object* object) {
        oop::Object** reference = newGlobalStrongReference(object);
        mLocalReferenceStack.back().references.push_back(reference);
        return reference;
    }

    void MemoryManager::safepoint(VM& vm) {
        if (vm.config().gc.pauseBudget == 0ns) mSafePointDeadline = std::nullopt;
        else mSafePointDeadline = vm.timeManager().now() + vm.config().gc.pauseBudget;

        bool didGCWork = false;

        if (mNursery.getFromLoadFactor() >= vm.config().gc.nurseryCollectionThreshold || mNurseryCollectState.phase != NurseryCollectPhase::Idle) {
            vm.scheduler().setGCRunning(true);
            nurseryCollect(vm);
            didGCWork = true;
        }

        if (mOldGenHeap.getLoadFactor() >= vm.config().gc.oldGenHeapCollectionThreshold || mOldGenHeap.isCollecting()) {
            vm.scheduler().setGCRunning(true);
            oldHeapCollect(vm);
            didGCWork = true;
        }

        if (!didGCWork) {
            vm.scheduler().setGCRunning(false);
        }

        if (didGCWork && mNurseryCollectState.phase == NurseryCollectPhase::Idle && !mOldGenHeap.isCollecting()) {
            vm.scheduler().setGCRunning(false);
        }

        if (!vm.scheduler().isGCRunning()) {
            vm.scheduler().safeDeleteExpiredTasks();
        }
    }

    void MemoryManager::queueFinalizer(VM& vm, oop::Object* object) {
        if (vm.config().gc.disableFinalizers) return;
        mFinalizerQueue.push_back(object);
    }

    oop::Object* MemoryManager::allocateRawObject(VM& vm, size_t size) {
        if (mNurseryCollectState.phase != NurseryCollectPhase::Idle) return mNursery.allocateInToSpace(size);
        else return mNursery.allocateInFromSpace(size);
    }

    bool MemoryManager::shouldPause(VM& vm) const {
        if (mSafePointDeadline && vm.timeManager().now() >= *mSafePointDeadline) {
            vm.debugLog(
                "GC",
                "Pause budget exceeded. Yielding"
            );
            return true;
        }
        return false;
    }

    void MemoryManager::nurseryCollect(VM& vm) {
        if (mNursery.getFromLoadFactor() < vm.config().gc.nurseryCollectionThreshold) return;

        auto& state = mNurseryCollectState;

        if (state.scan == nullptr) {
            mNursery.resetToSpace();
            state.scan = mNursery.toStart;

            vm.debugLog(
                "GC",
                "Nursery collection start | fromUsed={} bytes ({} load) | rememberedSetCount={}",
                mNursery.fromAllocPointer - mNursery.fromStart,
                mNursery.getFromLoadFactor(),
                mRememberedSet.size()
            );
        }

        auto& allTasks = vm.scheduler().allTasks();

        size_t taskCount = allTasks.size();

        int i = 0;

        while (state.phase != NurseryCollectPhase::Done) {
            // Fallthrough is intentional
            switch (state.phase) {
                case NurseryCollectPhase::Idle:
                    state.phase = NurseryCollectPhase::Roots;
                case NurseryCollectPhase::Roots: {
                    // scan the stack root set
                    for (; state.taskIndex < taskCount; state.taskIndex++) {
                        if ((++i & 255) == 0 && shouldPause(vm)) return;

                        executor::Task* task = allTasks[state.taskIndex];

                        //TODO: make this safer since the program could tear itself down and then this would track nonexistent frame pointers
                        if (state.currentFrame != nullptr) state.currentFrame = task->stack.getTop();
                        for (; state.currentFrame != nullptr; state.currentFrame = state.currentFrame->getPrev()) {
                            if ((++i & 255) == 0 && shouldPause(vm)) return;

                            executor::Frame& frame = *state.currentFrame;
                            uint16_t registerCount = frame.getFunction().getRegisterCount();
                            for (uint16_t j = 0; j < registerCount; j++) {
                                if (frame[j].isObject) {
                                    frame[j].obj = forward(vm, frame[j].obj);
                                }
                            }
                        }

                        task->completionFuture = forward(vm, task->completionFuture->asObject())->asFuture();
                    }

                    // scan the strong references
                    for (oop::Object** reference : mStrongReferences) {
                        *reference = forward(vm, *reference);
                    }

                    state.phase = NurseryCollectPhase::RememberedSet;
                    state.rememberedIndex = 0;

                    vm.debugLog(
                        "GC",
                        "Nursery collection | Roots phase finished"
                    );
                }
                case NurseryCollectPhase::RememberedSet: {
                    for (; state.rememberedIndex < mRememberedSet.size(); state.rememberedIndex++) {
                        if ((++i & 255) == 0 && shouldPause(vm)) return;

                        oop::Object* object = mRememberedSet[state.rememberedIndex];
                        bool hasYoungChild = false;
                        object->visitChildren([this, &hasYoungChild, &vm](oop::Object*& child) {
                            child = forward(vm, child);
                            if (mNursery.isInFromSpace(child)) hasYoungChild = true;
                        });

                        if (!hasYoungChild) {
                            vm.debugLog(
                                "GC",
                                "RememberedSet remove object={:016X}",
                                reinterpret_cast<uintptr_t>(object)
                            );

                            // hacky implementation, but should work
                            mRememberedSet[state.rememberedIndex] = mRememberedSet.back();
                            mRememberedSet.pop_back();
                            state.rememberedIndex--;
                        }
                    }

                    state.phase = NurseryCollectPhase::CheneyScan;

                    vm.debugLog(
                        "GC",
                        "Nursery collection | RememberedSet phase finished | remainingEntries={}",
                        mRememberedSet.size()
                    );
                }
                case NurseryCollectPhase::CheneyScan: {
                    while (state.scan < mNursery.toAllocPointer) {
                        if ((++i & 255) == 0 && shouldPause(vm)) return;

                        oop::Object* object = reinterpret_cast<oop::Object*>(state.scan);
                        object->visitChildren([this, &vm](oop::Object*& child) {
                            child = forward(vm, child);
                        });

                        state.scan += object->allocatedSize;
                    }

                    state.phase = NurseryCollectPhase::Done;

                    vm.debugLog(
                        "GC",
                        "Nursery collection | CheneyScan phase finished | scannedBytes={}",
                        state.scan - mNursery.toStart
                    );
                }
                case NurseryCollectPhase::Done:
                    break;
            }
        }

        finalizeDeadObjectsInNursery(vm);

        vm.debugLog(
            "GC",
            "Nursery collection finished | toUsed={} ({} load) bytes",
            mNursery.toAllocPointer - mNursery.toStart,
            mNursery.getToLoadFactor()
        );

        mNursery.swap();
        state = {}; // c++ my beloved

        if (mNursery.getFromLoadFactor() >= vm.config().memory.nurseryGrowthThreshold) {
            resizeNursery(vm);
        }
    }

    void MemoryManager::resizeNursery(VM& vm) {
        auto start = vm.timeManager().now();

        size_t newSize = std::min(static_cast<size_t>(static_cast<double>(mNursery.getSpaceSize()) * vm.config().memory.nurseryGrowthFactor), vm.config().memory.nurseryMaxSize);
        if (newSize == mNursery.getSpaceSize()) return;

        Nursery newNursery;
        if (!newNursery.init(newSize)) return; // TODO: exceptions

        vm.debugLog(
            "GC",
            "Resizing nursery {} -> {} | fromStart={:016X} | newFromStart={:016X}",
            mNursery.getSpaceSize(),
            newSize,
            reinterpret_cast<uintptr_t>(mNursery.fromStart),
            reinterpret_cast<uintptr_t>(newNursery.fromStart)
        );

        memcpy(newNursery.fromStart, mNursery.fromStart, mNursery.fromAllocPointer - mNursery.fromStart);
        newNursery.fromAllocPointer = newNursery.fromStart + (mNursery.fromAllocPointer - mNursery.fromStart);

        ptrdiff_t offset = newNursery.fromStart - mNursery.fromStart;

        auto& allTasks = vm.scheduler().allTasks();

        auto logResize = [&vm, offset](oop::Object* object) {
            /*
            vm.debugLog(
                "GC",
                "Resize fix | object={:016X} | newObject={:016X}",
                reinterpret_cast<uintptr_t>(object),
                reinterpret_cast<uintptr_t>(object) + offset
            );
            */
        };

        for (auto& task : allTasks) {
            executor::Stack& stack = task->stack;
            for (executor::Frame* framePtr = stack.getTop(); framePtr != nullptr; framePtr = framePtr->getPrev()) {
                executor::Frame& frame = *framePtr;
                uint16_t registerCount = frame.getFunction().getRegisterCount();
                for (uint16_t i = 0; i < registerCount; i++) {
                    if (frame[i].isObject) {
                        if (mNursery.isInFromSpace(frame[i].obj)) {
                            logResize(frame[i].obj);
                            frame[i].obj = reinterpret_cast<oop::Object*>(reinterpret_cast<uint8_t*>(frame[i].obj) + offset);
                        }
                    }
                }
            }
        }

        for (oop::Object** reference : mStrongReferences) {
            if (mNursery.isInFromSpace(*reference)) {
                logResize(*reference);
                *reference = reinterpret_cast<oop::Object*>(reinterpret_cast<uint8_t*>(*reference) + offset);
            }
        }

        for (oop::Object* object : mRememberedSet) {
            object->visitChildren([this, &logResize, offset](oop::Object*& child) {
                if (mNursery.isInFromSpace(child)) {
                    logResize(child);
                    child = reinterpret_cast<oop::Object*>(reinterpret_cast<uint8_t*>(child) + offset);
                }
            });
        }

        uint8_t* scan = newNursery.fromStart;

        while (scan < newNursery.fromAllocPointer) {
            oop::Object* object = reinterpret_cast<oop::Object*>(scan);
            object->visitChildren([this, &logResize, offset](oop::Object*& child) {
                if (mNursery.isInFromSpace(child)) {
                    logResize(child);
                    child = reinterpret_cast<oop::Object*>(reinterpret_cast<uint8_t*>(child) + offset);
                }
            });

            scan += object->allocatedSize;
        }

        vm.debugLog(
            "GC",
            "Nursery resized | offset={} | newFromUsed={} | time={}",
            offset,
            newNursery.fromAllocPointer - newNursery.fromStart,
            vm.timeManager().now() - start
        );

        mNursery = std::move(newNursery);
    }

    oop::Object* MemoryManager::forward(VM& vm, oop::Object* object) {
        if (object == nullptr) return nullptr;
        if (!mNursery.isInFromSpace(object)) return object;
        if (object->forward != nullptr) {
            /*
            vm.debugLog(
                "GC",
                "Forward reuse {:016X} -> {:016X}",
                reinterpret_cast<uintptr_t>(object),
                reinterpret_cast<uintptr_t>(object->forward)
            );
            */
            return object->forward;
        }

        oop::Object* newObject;
        if (object->age >= vm.config().gc.promotionAge) newObject = promote(vm, object);
        else newObject = mNursery.allocateInToSpace(object->allocatedSize);

        memcpy(newObject, object, object->allocatedSize);
        newObject->age++;

        BIBBLEVM_ASSERT(object->allocatedSize != 0);
        BIBBLEVM_ASSERT(newObject->allocatedSize != 0);

        /*
        vm.debugLog(
            "GC",
            "Forward copy {:016X} -> {:016X} | size={}",
            reinterpret_cast<uintptr_t>(object),
            reinterpret_cast<uintptr_t>(newObject),
            object->allocatedSize
        );
        */

        object->forward = newObject;
        return newObject;
    }

    oop::Object* MemoryManager::promote(VM& vm, oop::Object* object) {
        oop::Object* newObject = mOldGenHeap.allocate(vm, object->allocatedSize);
        /*
        vm.debugLog(
            "GC",
            "Object promotion {:016X} -> {:016X} | size={}",
            reinterpret_cast<uintptr_t>(object),
            reinterpret_cast<uintptr_t>(newObject),
            object->allocatedSize
        );
        */
        return newObject;
    }

    void MemoryManager::finalizeDeadObjectsInNursery(VM& vm) {
        if (vm.config().gc.disableFinalizers) return;

        uint8_t* scan = mNursery.fromStart;

        while (scan < mNursery.fromAllocPointer) {
            oop::Object* object = reinterpret_cast<oop::Object*>(scan);

            if (object->forward == nullptr && object->kind == oop::ObjectKind::Instance) {
                if (object->asInstance()->clas->hasFinalizer()) {
                    oop::Object* newObject = forward(vm, object);
                    mFinalizerQueue.push_back(newObject);

                    vm.debugLog(
                        "GC",
                        "Finalizer queued | object={:016X}",
                        reinterpret_cast<uintptr_t>(object)
                    );
                }
            }

            scan += object->allocatedSize;
        }
    }

    void MemoryManager::oldHeapCollect(VM& vm) {
        mOldGenHeap.startCollection(vm); // this function internally checks if it's already active so just remember to update this if that's ever changed
        mOldGenHeap.stepCollection(vm, mSafePointDeadline);
    }
}
