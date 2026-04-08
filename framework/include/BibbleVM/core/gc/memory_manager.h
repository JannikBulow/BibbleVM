// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_CORE_GC_MEMORY_MANAGER_H
#define BIBBLEVM_CORE_GC_MEMORY_MANAGER_H 1

#include "BibbleVM/core/executor/stack.h"

#include "BibbleVM/core/gc/nursery.h"

#include "BibbleVM/core/oop/object.h"

#include "BibbleVM/util/time_manager.h"

#include "BibbleVM/api.h"

#include <array>
#include <chrono>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace bibblevm::gc {
    enum class Phase {
        Idle,
        NurseryCollecting,
        OldHeapCollecting,
        Done,
    };

    struct Root {
        using Iterator = oop::Object**;

        oop::Object** base;
        size_t count;

        Root(oop::Object** base, size_t count) : base(base), count(count) {}
        Root(std::vector<oop::Object*>& v) : base(v.data()), count(v.size()) {}

        Iterator begin() const { return base; }
        Iterator end() const { return base + count; }
    };

    class BIBBLEVM_EXPORT MemoryManager {
    public:
        MemoryManager() = default;

        bool init(VM& vm);

        // Be careful because this function will create a Type with only the given kind, which can break with some object type kinds.
        oop::TypeID getPrimitiveType(oop::Type::Kind kind);

        oop::TypeID getInstanceType(oop::Class* clas);
        oop::TypeID getArrayType(oop::TypeID baseType);
        oop::TypeID getStringType();
        oop::TypeID getFutureType();

        const oop::Type* getType(oop::TypeID id) const;

        oop::Object* allocateInstance(oop::Class* clas);

        oop::Object* allocateArray(oop::TypeID baseType, ULong length);
        oop::Object* reallocateArray(oop::Object* array, ULong newLength);

        oop::Object* allocateString(ULong lengthBytes);
        oop::Object* allocateString(std::string_view copy);
        oop::Object* allocateImmortalString(ULong lengthBytes);
        oop::Object* allocateImmortalString(std::string_view copy);

        oop::Object* allocateFuture();

        void addRoot(oop::Object** root) { addRoot({root, 1}); }
        void addRoot(Root root);

        // Call this at every safe point throughout the program and the memory manager will decide if it should use this safe point to run a collection cycle or not.
        void safepoint(VM& vm);

    private:
        enum class NurseryCollectPhase {
            Roots,
            RememberedSet,
            CheneyScan,
            Done,
        };

        union State {
            struct {
                uint8_t* scan = nullptr;

                NurseryCollectPhase phase = NurseryCollectPhase::Roots;

                size_t stackIndex = 0;
                executor::Frame* currentFrame = nullptr;
                size_t rootIndex = 0;
                size_t rememberedIndex = 0;
            } nursery;
        };

        std::vector<oop::Type> mTypes;
        std::array<std::optional<oop::TypeID>, oop::Type::Kind::Count> mPrimitiveTypes{};
        std::unordered_map<oop::Class*, oop::TypeID> mInstanceTypes;
        std::unordered_map<oop::TypeID, oop::TypeID> mArrayTypes;

        TimeManager<>::TimePoint mSafePointStart;

        Nursery mNursery{};

        std::vector<oop::Object*> mFinalizerQueue;

        std::vector<Root> mRoots;
        std::vector<oop::Object*> mRememberedSet;

        Phase mPhase = Phase::Idle;
        State mState{};

        bool shouldPause(VM& vm) const;

        void nurseryCollect(VM& vm);
        void resizeNursery(VM& vm);
        oop::Object* forward(VM& vm, oop::Object* object);
        void finalizeDeadObjectsInNursery(VM& vm);

        void oldHeapCollect(VM& vm);
    };
}

#endif // BIBBLEVM_CORE_GC_MEMORY_MANAGER_H
