// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/core/gc/memory_manager.h"

#include "BibbleVM/core/oop/class.h"

#include "BibbleVM/vm.h"

namespace bibblevm::gc {
    bool MemoryManager::init(VM& vm) {
        mPauseBudget = 0ns;
        return mNursery.init(512 * 1024 * 1024);
    }

    oop::TypeID MemoryManager::getPrimitiveType(oop::Type::Kind kind) {
        auto& id = mPrimitiveTypes[kind];
        if (id.has_value()) return id.value();

        id = mTypes.size();
        mTypes.emplace_back(kind);

        return id.value();
    }

    oop::TypeID MemoryManager::getInstanceType(oop::Class* clas) {
        auto it = mInstanceTypes.find(clas);
        if (it != mInstanceTypes.end()) {
            return it->second;
        }

        oop::TypeID id = mTypes.size();
        mTypes.emplace_back(oop::Type::Instance);
        mTypes.back().instanceClass = clas;

        mInstanceTypes[clas] = id;

        return id;
    }

    oop::TypeID MemoryManager::getArrayType(oop::TypeID baseType) {
        auto it = mArrayTypes.find(baseType);
        if (it != mArrayTypes.end()) {
            return it->second;
        }

        oop::TypeID id = mTypes.size();
        mTypes.emplace_back(oop::Type::Array);
        mTypes.back().baseType = getType(baseType);

        mArrayTypes[baseType] = id;

        return id;
    }

    oop::TypeID MemoryManager::getStringType() {
        return getPrimitiveType(oop::Type::String);
    }

    oop::TypeID MemoryManager::getFutureType() {
        oop::TypeID id = getPrimitiveType(oop::Type::Future);
        return id;
    }

    const oop::Type* MemoryManager::getType(oop::TypeID id) const {
        return &mTypes[id];
    }

    oop::Object* MemoryManager::allocateInstance(oop::Class* clas) {
        oop::Object* object = mNursery.allocate(clas->getMemorySize());
        if (object != nullptr) {
            object->type = getInstanceType(clas);
            object->asInstance()->finalizerQueued = false;
        }
        return object;
    }

    oop::Object* MemoryManager::allocateArray(oop::TypeID baseType, ULong length) {
        return nullptr;
    }

    oop::Object* MemoryManager::reallocateArray(oop::Object* array, ULong newLength) {
        return nullptr;
    }

    oop::Object* MemoryManager::allocateString(ULong lengthBytes) {
        oop::Object* object = mNursery.allocate(sizeof(oop::StringObject) + lengthBytes);
        if (object != nullptr) {
            object->type = getStringType();
            object->asString()->lengthBytes = lengthBytes;
        }
        return object;
    }

    oop::Object* MemoryManager::allocateString(std::string_view copy) {
        oop::Object* object = mNursery.allocate(sizeof(oop::StringObject) + copy.size());
        if (object != nullptr) {
            object->type = getStringType();
            object->asString()->lengthBytes = copy.size();
            memcpy(object->asString()->bytes, copy.data(), copy.size());
        }
        return object;
    }

    oop::Object* MemoryManager::allocateImmortalString(ULong lengthBytes) {
        //TODO: refine this with custom immortal heap
        oop::StringObject* object = static_cast<oop::StringObject*>(malloc(sizeof(oop::StringObject) + lengthBytes));
        new(object) oop::StringObject();
        object->asObject()->type = getStringType();
        object->lengthBytes = lengthBytes;
        return object->asObject();
    }

    oop::Object* MemoryManager::allocateImmortalString(std::string_view copy) {
        oop::StringObject* object = static_cast<oop::StringObject*>(malloc(sizeof(oop::StringObject) + copy.size()));
        new(object) oop::StringObject();
        object->asObject()->type = getStringType();
        object->lengthBytes = copy.size();
        memcpy(object->bytes, copy.data(), copy.size());
        return object->asObject();
    }

    oop::Object* MemoryManager::allocateFuture() {
        oop::Object* object = mNursery.allocate(sizeof(oop::Future));
        if (object != nullptr) {
            object->type = getFutureType();
            object->asFuture()->ready = false;
            object->asFuture()->waiters = nullptr;
            object->asFuture()->waiterCount = 0;
        }
        return object;
    }

    void MemoryManager::addRoot(Root root) {
        mRoots.push_back(root);
    }

    void MemoryManager::safepoint(VM& vm) {
        switch (mPhase) {
            case Phase::Idle:
                mPhase = Phase::NurseryCollecting; // TODO: do some real shit to determine if we need to collect yet
                vm.scheduler().setGCRunning(true);
            case Phase::NurseryCollecting:
                nurseryCollect(vm);
                if (shouldPause(vm)) break;
            case Phase::OldHeapCollecting:
                oldHeapCollect(vm);
                if (shouldPause(vm)) break;
            case Phase::Done:
                vm.scheduler().safeDeleteExpiredTasks();
                vm.scheduler().setGCRunning(false);
                mPhase = Phase::Idle;
                break;
        }

        for (oop::Object* object : mFinalizerQueue) {
            Value asValue{};
            asValue.type = object->type;
            asValue.obj = object;

            oop::Class* clas = mTypes[object->type].instanceClass;
            vm.scheduler().schedule(vm, *clas->dispatchMethod(clas->getFinalizer()), &asValue);
        }
    }

    bool MemoryManager::shouldPause(VM& vm) const {
        if (mPauseBudget == Nanoseconds::zero()) return false;
        return vm.timeManager().hasPassed(mSafePointStart, mPauseBudget);
    }

    void MemoryManager::nurseryCollect(VM& vm) {
        auto& state = mState.nursery;

        if (state.scan == nullptr) state.allocPointer = mNursery.toStart;

        auto& allTasks = vm.scheduler().allTasks();

        size_t stackCount = allTasks.size();
        size_t rootCount = mRoots.size();

        while (state.phase != NurseryCollectPhase::Done) {
            // Fallthrough is intentional
            switch (state.phase) {
                case NurseryCollectPhase::Roots: {
                    // scan the stack root set
                    for (; state.stackIndex < stackCount; state.stackIndex++) {
                        if (shouldPause(vm)) return;

                        executor::Stack& stack = allTasks[state.stackIndex]->stack;
                        for (; state.currentFrame != nullptr; state.currentFrame = state.currentFrame->getPrev()) {
                            if (shouldPause(vm)) return;

                            executor::Frame& frame = *state.currentFrame;
                            uint16_t registerCount = frame.getFunction().getRegisterCount();
                            for (uint16_t i = 0; i < registerCount; i++) {
                                if (mTypes[frame[i].type].isObjectType()) {
                                    frame[i].obj = forward(frame[i].obj);
                                }
                            }
                        }
                    }

                    // scan the third party root set
                    for (; state.rootIndex < rootCount; state.rootIndex++) {
                        if (shouldPause(vm)) return;

                        for (oop::Object*& object : mRoots[state.rootIndex]) {
                            object = forward(object);
                        }
                    }

                    state.phase = NurseryCollectPhase::RememberedSet;
                    state.rememberedIndex = 0;
                }
                case NurseryCollectPhase::RememberedSet: {
                    for (; state.rememberedIndex < mRememberedSet.size(); state.rememberedIndex++) {
                        if (shouldPause(vm)) return;

                        oop::Object* object = mRememberedSet[state.rememberedIndex];
                        object->visitChildren(&mTypes[object->type], [this](oop::Object*& child) {
                            child = forward(child);
                        });
                    }

                    state.phase = NurseryCollectPhase::CheneyScan;
                    if (state.scan == nullptr) state.scan = mNursery.toStart;
                }
                case NurseryCollectPhase::CheneyScan: {
                    while (state.scan < state.allocPointer) {
                        if (shouldPause(vm)) return;

                        oop::Object* object = reinterpret_cast<oop::Object*>(state.scan);
                        object->visitChildren(&mTypes[object->type], [this](oop::Object*& child) {
                            child = forward(child);
                        });

                        state.scan += object->allocatedSize;
                    }

                    state.phase = NurseryCollectPhase::Done;
                }
                case NurseryCollectPhase::Done:
                    break;
            }
        }

        finalizeDeadObjectsInNursery(vm);

        mNursery.swap();
        mNursery.allocPointer = state.allocPointer;
        state = {}; // c++ my beloved

        mPhase = Phase::OldHeapCollecting;
    }

    oop::Object* MemoryManager::forward(oop::Object* object) {
        if (!mNursery.isInFromSpace(object)) return object;
        if (object->forward != nullptr) return object->forward;

        oop::Object* newObject = reinterpret_cast<oop::Object*>(mState.nursery.allocPointer);
        memcpy(newObject, object, object->allocatedSize);
        mState.nursery.allocPointer += object->allocatedSize;

        object->forward = newObject;
        return newObject;
    }

    void MemoryManager::finalizeDeadObjectsInNursery(VM& vm) {
        uint8_t* scan = mNursery.fromStart;

        while (scan < mNursery.allocPointer) {
            oop::Object* object = reinterpret_cast<oop::Object*>(scan);

            if (object->forward == nullptr && mTypes[object->type].kind == oop::Type::Instance) {
                if (mTypes[object->type].instanceClass->hasFinalizer()) {
                    object->asInstance()->finalizerQueued = true;
                    oop::Object* newObject = forward(object);
                    mFinalizerQueue.push_back(newObject);
                }
            }

            scan += object->allocatedSize;
        }
    }

    void MemoryManager::oldHeapCollect(VM& vm) {

    }
}
