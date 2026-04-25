// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_CORE_GC_MEMORY_MANAGER_H
#define BIBBLEVM_CORE_GC_MEMORY_MANAGER_H 1

#include "BibbleVM/allocator/arena.h"

#include "BibbleVM/core/executor/stack.h"

#include "BibbleVM/core/gc/nursery.h"
#include "BibbleVM/core/gc/old_gen_heap.h"

#include "BibbleVM/core/oop/object.h"

#include "BibbleVM/util/time_manager.h"

#include "BibbleVM/api.h"

#include <chrono>
#include <string_view>
#include <unordered_set>
#include <vector>

namespace bibblevm::gc {
    constexpr uint8_t NURSERY_ID = 1;
    constexpr uint8_t OLD_GEN_ID = 2;
    constexpr uint8_t IMMORTAL_ID = 3;
    constexpr uint8_t LOH_ID = 4;

    constexpr uint8_t NOT_OLD_GEN_ID = 253;

    class BIBBLEVM_EXPORT MemoryManager {
    friend class OldGenHeap;
    public:
        MemoryManager();

        bool init(VM& vm);

        oop::Object* allocateInstance(VM& vm, oop::Class* clas);

        oop::Object* allocateArray(VM& vm, oop::Type baseType, ULong length);
        oop::Object* reallocateArray(VM& vm, oop::Object* array, ULong newLength);

        oop::Object* allocateString(VM& vm, ULong lengthBytes);
        oop::Object* allocateString(VM& vm, std::string_view copy);
        oop::Object* allocateImmortalString(VM& vm, ULong lengthBytes);
        oop::Object* allocateImmortalString(VM& vm, std::string_view copy);

        oop::Object* allocateFuture(VM& vm);

        void writeBarrier(oop::Object* object, oop::Object* child);

        const std::unordered_set<oop::Object**>& strongReferences() const { return mStrongReferences; }

        oop::Object** newGlobalStrongReference(oop::Object* object);
        void deleteGlobalStrongReference(oop::Object** reference);

        void pushLocalReferenceFrame(size_t initialSize);
        void popLocalReferenceFrame();
        oop::Object** newLocalStrongReference(oop::Object* object);

        // Call this at every safe point throughout the program and the memory manager will decide if it should use this safe point to run a collection cycle or not.
        void safepoint(VM& vm);

    private:
        struct LocalReferenceFrame {
            std::vector<oop::Object**> references;
        };

        enum class NurseryCollectPhase {
            Idle,
            Roots,
            RememberedSet,
            CheneyScan,
            Done,
        };

        struct NurseryCollectState {
            uint8_t* scan = nullptr;

            NurseryCollectPhase phase = NurseryCollectPhase::Idle;

            size_t taskIndex = 0;
            executor::Frame* currentFrame = nullptr;
            size_t rememberedIndex = 0;
        };

        std::optional<TimeManager<>::TimePoint> mSafePointDeadline;

        GrowingArenaAllocator mImmortalAllocator;
        Nursery mNursery{};
        OldGenHeap mOldGenHeap{};

        std::vector<oop::Object*> mFinalizerQueue;

        std::unordered_set<oop::Object**> mStrongReferences;
        std::vector<LocalReferenceFrame> mLocalReferenceStack;

        std::vector<oop::Object*> mRememberedSet;

        NurseryCollectState mNurseryCollectState{};

        void queueFinalizer(VM& vm, oop::Object* object);

        oop::Object* allocateRawObject(VM& vm, size_t size);

        bool shouldPause(VM& vm) const;

        void nurseryCollect(VM& vm);
        void resizeNursery(VM& vm);
        oop::Object* forward(VM& vm, oop::Object* object);
        oop::Object* promote(VM& vm, oop::Object* object);
        void finalizeDeadObjectsInNursery(VM& vm);

        void oldHeapCollect(VM& vm);
    };
}

#endif // BIBBLEVM_CORE_GC_MEMORY_MANAGER_H
