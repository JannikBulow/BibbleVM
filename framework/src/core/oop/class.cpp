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

    Value Class::readField(Instance* instance, const Field* field) const {
        const uint8_t* src = instance->fieldBytes + field->memoryOffset;
        Value value{};

        switch (field->type) {
            case Type::Byte:
                std::memcpy(&value.b, src, sizeof(Byte));
                break;
            case Type::UByte:
                std::memcpy(&value.ub, src, sizeof(UByte));
                break;
            case Type::Short:
                std::memcpy(&value.s, src, sizeof(Short));
                break;
            case Type::UShort:
                std::memcpy(&value.us, src, sizeof(UShort));
                break;
            case Type::Int:
                std::memcpy(&value.i, src, sizeof(Int));
                break;
            case Type::UInt:
                std::memcpy(&value.ui, src, sizeof(UInt));
                break;
            case Type::Long:
                std::memcpy(&value.l, src, sizeof(Long));
                break;
            case Type::ULong:
                std::memcpy(&value.ul, src, sizeof(ULong));
                break;
            case Type::Float:
                std::memcpy(&value.f, src, sizeof(Float));
                break;
            case Type::Double:
                std::memcpy(&value.d, src, sizeof(Double));
                break;
            case Type::Handle:
                std::memcpy(&value.h, src, sizeof(Handle));
                break;
            case Type::Reference:
                std::memcpy(&value.obj, src, sizeof(Object*));
                break;
            case Type::ModuleRef:
                std::memcpy(&value.mi, src, sizeof(executor::Module*));
                break;
            case Type::ClassRef:
                std::memcpy(&value.ci, src, sizeof(Class*));
                break;
            case Type::FieldRef:
                std::memcpy(&value.fi, src, sizeof(Field*));
                break;
            case Type::MethodRef:
                std::memcpy(&value.mei, src, sizeof(Method*));
                break;
            case Type::FunctionRef:
                std::memcpy(&value.fni, src, sizeof(executor::Function*));
                break;
            case Type::Count: break;
        }

        return value;

    }

    void Class::writeField(Instance* instance, const Field* field, Value value) {
        uint8_t* dest = instance->fieldBytes + field->memoryOffset;

        switch (field->type) {
            case Type::Byte:
                std::memcpy(dest, &value.b, sizeof(Byte));
                break;
            case Type::UByte:
                std::memcpy(dest, &value.ub, sizeof(UByte));
                break;
            case Type::Short:
                std::memcpy(dest, &value.s, sizeof(Short));
                break;
            case Type::UShort:
                std::memcpy(dest, &value.us, sizeof(UShort));
                break;
            case Type::Int:
                std::memcpy(dest, &value.i, sizeof(Int));
                break;
            case Type::UInt:
                std::memcpy(dest, &value.ui, sizeof(UInt));
                break;
            case Type::Long:
                std::memcpy(dest, &value.l, sizeof(Long));
                break;
            case Type::ULong:
                std::memcpy(dest, &value.ul, sizeof(ULong));
                break;
            case Type::Float:
                std::memcpy(dest, &value.f, sizeof(Float));
                break;
            case Type::Double:
                std::memcpy(dest, &value.d, sizeof(Double));
                break;
            case Type::Handle:
                std::memcpy(dest, &value.h, sizeof(Handle));
                break;
            case Type::Reference:
                std::memcpy(dest, &value.obj, sizeof(Object*));
                break;
            case Type::ModuleRef:
                std::memcpy(dest, &value.mi, sizeof(executor::Module*));
                break;
            case Type::ClassRef:
                std::memcpy(dest, &value.ci, sizeof(Class*));
                break;
            case Type::FieldRef:
                std::memcpy(dest, &value.fi, sizeof(Field*));
                break;
            case Type::MethodRef:
                std::memcpy(dest, &value.mei, sizeof(Method*));
                break;
            case Type::FunctionRef:
                std::memcpy(dest, &value.fni, sizeof(executor::Function*));
                break;
            case Type::Count:
                break;
        }
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
