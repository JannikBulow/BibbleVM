// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_CORE_OOP_CLASS_H
#define BIBBLEVM_CORE_OOP_CLASS_H 1

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
        Class(String name, Class* superClass, std::span<Field> fields, std::span<Method> methods, std::span<Method*> unlinkedVtable); // constructor takes unlinked vtable because the linker controls the memory and the Class uses it

        String getName() const { return mName; }
        Class* getSuperClass() const { return mSuperClass; }
        Method* getFinalizer() const { return mFinalizer; }
        bool hasFinalizer() const { return mFinalizer != nullptr; }
        uint64_t getMemorySize() const { return mMemorySize; }
        uint16_t getObjectFieldCount() const { return mObjectFieldCount; }

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
        std::span<Method> mMethods;
        std::span<Method*> mVtable;

        Method* mFinalizer;

        uint64_t mMemorySize;

        uint16_t mObjectFieldCount;

        void sortFields();
        void orderFieldBuckets(std::array<FieldBucket, Type::Count>& buckets);
        static void orderFieldBucket(FieldBucket& bucket, uint32_t size, uint64_t& offset);

        void createVtable();
    };
}

#endif // BIBBLEVM_CORE_OOP_CLASS_H
