// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/core/oop/class.h"

#include <algorithm>

namespace bibblevm::oop {
    constexpr size_t AlignUp(size_t value, size_t alignment) {
        return (value + (alignment - 1)) & ~(alignment - 1);
    }

    Class::Class(String name)
        : mName(name) {}

    Class::Class(String name, Class* superClass, std::span<Field> fields, std::span<Method> methods, GrowingArenaAllocator& arena)
        : mName(name)
        , mSuperClass(superClass)
        , mFields(fields)
        , mMethods(methods)
        , mFinalizer(nullptr) {
        if (mSuperClass != nullptr) {
            mMemorySize = mSuperClass->mMemorySize;
        } else {
            mMemorySize = 0;
        }

        sortFields(arena);
        createVtable(arena);

        mFinalizer = getMethod(".finalize");
    }

    const Field* Class::getField(String name) const {
        for (const auto& field : mFields) {
            if (field.name == name) return &field;
        }
        return nullptr;
    }

    const Field* Class::getField(std::string_view name) const {
        for (const auto& field : mFields) {
            if (field.name == name) return &field;
        }
        return nullptr;
    }

    const Method* Class::getMethod(String name) const {
        for (const auto& method : mMethods) {
            if (method.name == name) return &method;
        }
        return nullptr;
    }

    const Method* Class::getMethod(std::string_view name) const {
        for (const auto& method : mMethods) {
            if (method.name == name) return &method;
        }
        return nullptr;
    }

    executor::Function* Class::dispatchMethod(const Method* method) const {
        return mVtable[method->vtableIndex]->function;
    }

    bool Class::isAssignableTo(Class* other) const {
        if (other == nullptr) return false;

        const Class* current = this;
        do { // This snippet of code has earned the award of being my first usage of do while.
            if (current == other) return true;
            current = current->mSuperClass;
        } while (current != nullptr);

        return false;
    }

    void Class::sortFields(GrowingArenaAllocator& arena) {
        std::array<FieldBucket, static_cast<size_t>(Type::Count)> buckets;
        for (auto& field : mFields) {
            FieldBucket& bucket = buckets[static_cast<size_t>(field.type)];
            bucket.push_back(&field);
        }

        orderFieldBuckets(buckets);

        // Should be correct
        auto& referenceBucket = buckets[static_cast<size_t>(Type::Reference)];
        if (!referenceBucket.empty()) {
            if (mSuperClass != nullptr) {
                mReferenceRegions = {arena.allocate<ReferenceRegion>(mSuperClass->mReferenceRegions.size()) + 1, mSuperClass->mReferenceRegions.size() + 1};
                std::ranges::copy(mSuperClass->mReferenceRegions, mReferenceRegions.begin());
            } else {
                mReferenceRegions = {arena.allocate<ReferenceRegion>(1), 1};
            }

            mReferenceRegions.back().offset = referenceBucket[0]->memoryOffset;
            mReferenceRegions.back().count = referenceBucket.size();
        } else {
            if (mSuperClass != nullptr && !mSuperClass->mReferenceRegions.empty()) {
                mReferenceRegions = {arena.allocate<ReferenceRegion>(mSuperClass->mReferenceRegions.size()), mSuperClass->mReferenceRegions.size()};
                std::ranges::copy(mSuperClass->mReferenceRegions, mReferenceRegions.begin());
            }
        }
    }

    void Class::orderFieldBuckets(std::array<FieldBucket, static_cast<size_t>(Type::Count)>& buckets) {
        uint64_t offset = mMemorySize;
        offset = AlignUp(offset, 8);

#define ORDER_FIELD_BUCKET(type, size) orderFieldBucket(buckets[static_cast<size_t>(type)], size, offset)
        ORDER_FIELD_BUCKET(Type::Reference, 8);

        offset = AlignUp(offset, 8);

        ORDER_FIELD_BUCKET(Type::Handle, 8);
        ORDER_FIELD_BUCKET(Type::ModuleRef, 8);
        ORDER_FIELD_BUCKET(Type::ClassRef, 8);
        ORDER_FIELD_BUCKET(Type::FieldRef, 8);
        ORDER_FIELD_BUCKET(Type::MethodRef, 8);
        ORDER_FIELD_BUCKET(Type::FunctionRef, 8);
        ORDER_FIELD_BUCKET(Type::Double, 8);
        ORDER_FIELD_BUCKET(Type::ULong, 8);
        ORDER_FIELD_BUCKET(Type::Long, 8);
        ORDER_FIELD_BUCKET(Type::Float, 4);
        ORDER_FIELD_BUCKET(Type::UInt, 4);
        ORDER_FIELD_BUCKET(Type::Int, 4);
        ORDER_FIELD_BUCKET(Type::UShort, 2);
        ORDER_FIELD_BUCKET(Type::Short, 2);
        ORDER_FIELD_BUCKET(Type::UByte, 1);
        ORDER_FIELD_BUCKET(Type::Byte, 1);
#undef  ORDER_FIELD_BUCKET

        mMemorySize = offset;
    }

    void Class::orderFieldBucket(FieldBucket& bucket, uint32_t size, uint64_t& offset) {
        uint64_t tempOffset = offset;

        for (Field* field : bucket) {
            field->memoryOffset = tempOffset;
            tempOffset += size;
        }

        offset = tempOffset;
    }

    void Class::createVtable(GrowingArenaAllocator& arena) {
        if (mSuperClass != nullptr) {
            size_t size = mSuperClass->mVtable.size();
            for (auto& method : mMethods) {
                auto it = std::ranges::find_if(mSuperClass->mVtable, [&method](Method* m) {
                    return m->name == method.name;
                });

                if (it == mSuperClass->mVtable.end()) {
                    size++;
                }
            }

            mVtable = {arena.allocate<Method*>(size), size};
            std::ranges::fill(mVtable, nullptr);
            std::ranges::copy(mSuperClass->mVtable, mVtable.begin());

            size_t i = mSuperClass->mVtable.size();
            for (auto& method : mMethods) {
                auto it = std::ranges::find_if(mVtable, [&method](Method* m) {
                    return m->name == method.name;
                });

                if (it != mVtable.end()) {
                    auto index = std::distance(mVtable.begin(), it);
                    mVtable[index] = &method;
                    method.vtableIndex = index;
                } else {
                    mVtable[i] = &method;
                    method.vtableIndex = i;
                    i++;
                }
            }
        } else {
            mVtable = {arena.allocate<Method*>(mMethods.size()), mMethods.size()};
            for (uint16_t i = 0; i < mMethods.size(); i++) {
                mVtable[i] = &mMethods[i];
            }
        }
    }
}
