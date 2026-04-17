// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_CORE_OOP_CLASS_H
#define BIBBLEVM_CORE_OOP_CLASS_H 1

#include "BibbleVM/allocator/arena.h"

#include "BibbleVM/core/oop/field.h"
#include "BibbleVM/core/oop/method.h"

#include <array>
#include <cstdint>
#include <span>
#include <vector>

namespace bibblevm::executor {
    class Function;
}

namespace bibblevm::oop {
    class BIBBLEVM_EXPORT Class {
    public:
        struct ReferenceRegion {
            uint64_t offset;
            uint16_t count;
        };

        Class(String name, Class* superClass, std::span<Field> fields, std::span<Method> methods, GrowingArenaAllocator& arena);

        String getName() const { return mName; }
        Class* getSuperClass() const { return mSuperClass; }
        Method* getFinalizer() const { return mFinalizer; }
        bool hasFinalizer() const { return mFinalizer != nullptr; }
        uint64_t getMemorySize() const { return mMemorySize; }
        uint64_t getTotalSize() const { return sizeof(Instance) + mMemorySize; }

        constexpr void visitReferenceRegions(auto visitor) {
            for (const auto& region : mReferenceRegions) {
                visitor(region);
            }
        }

        const Field* getField(String name) const;
        const Field* getField(std::string_view name) const;

        const Method* getMethod(String name) const;
        const Method* getMethod(std::string_view name) const;

        executor::Function* dispatchMethod(const Method* method) const;

        bool isAssignableTo(Class* other) const;

    private:
        using FieldBucket = std::vector<Field*>;

        String mName;
        Class* mSuperClass;

        std::span<Field> mFields;
        std::span<ReferenceRegion> mReferenceRegions;
        std::span<Method> mMethods;
        std::span<Method*> mVtable;

        Method* mFinalizer;

        uint64_t mMemorySize;

        void sortFields(GrowingArenaAllocator& arena);
        void orderFieldBuckets(std::array<FieldBucket, static_cast<size_t>(Type::Count)>& buckets);
        static void orderFieldBucket(FieldBucket& bucket, uint32_t size, uint64_t& offset);

        void createVtable(GrowingArenaAllocator& arena);
    };
}

#endif // BIBBLEVM_CORE_OOP_CLASS_H
