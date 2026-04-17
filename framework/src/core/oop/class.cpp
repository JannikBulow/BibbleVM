// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/core/oop/class.h"

#include <algorithm>

namespace bibblevm::oop {
    Class::Class(String name, Class* superClass, std::span<Field> fields, std::span<Method> methods, std::span<Method*> unlinkedVtable)
        : mName(name)
        , mSuperClass(superClass)
        , mFields(fields)
        , mMethods(methods)
        , mVtable(unlinkedVtable)
        , mFinalizer(nullptr)
        , mObjectFieldCount(0) {
        if (mSuperClass != nullptr) {
            mMemorySize = mSuperClass->mMemorySize;
            std::ranges::copy(mSuperClass->mVtable, mVtable.begin());
        } else {
            mMemorySize = sizeof(Instance);
        }

        sortFields();
        createVtable();
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

    void Class::sortFields() {
        std::array<FieldBucket, static_cast<size_t>(Type::Count)> buckets;
        for (auto& field : mFields) {
            FieldBucket& bucket = buckets[static_cast<size_t>(field.type)];
            bucket.push_back(&field);
        }

        orderFieldBuckets(buckets);
    }

    void Class::orderFieldBuckets(std::array<FieldBucket, static_cast<size_t>(Type::Count)>& buckets) {
        uint64_t offset = mMemorySize;
        offset = (offset + 7) & ~7;

#define ORDER_FIELD_BUCKET(type, size) orderFieldBucket(buckets[static_cast<size_t>(type)], size, offset)
        ORDER_FIELD_BUCKET(Type::Reference, 8);
        ORDER_FIELD_BUCKET(Type::ModuleRef, 8);
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

        for (auto& field : bucket) {
            field->memoryOffset = tempOffset;
            tempOffset += size;
        }

        offset = tempOffset;
    }

    void Class::createVtable() {
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
                method.vtableIndex = i;
                mVtable[i++] = &method;
            }
        }
    }
}
