// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_CORE_OOP_ARRAY_VIEW_H
#define BIBBLEVM_CORE_OOP_ARRAY_VIEW_H 1

#include "BibbleVM/core/gc/memory_manager.h"

#include "BibbleVM/core/oop/object.h"

namespace bibblevm::oop {
    template<class T>
    class ArrayView {
    public:
        ArrayView(Array* array) : mArray(array) {}

        ULong length() const { return mArray->length; }
        T& operator[](ULong index) { return data()[index]; }
        const T& operator[](ULong index) const { return data()[index]; }

    private:
        Array* mArray;

        T* data() const {
            return reinterpret_cast<T*>(mArray->elementBytes);
        }
    };

    // A wrapper of raw data used to make a vector on GC managed memory
    template<class T>
    class GrowingArrayView {
    public:
        using Iterator = T*;

        GrowingArrayView(gc::MemoryManager& memoryManager, Object*& array, ULong& size) : mMemoryManager(memoryManager), mArray(array), mSize(size) {}

        ULong size() const { return mSize; }
        ULong capacity() const { return mArray == nullptr ? 0 : mArray->asArray()->length; }

        T& operator[](ULong index) { return data()[index]; }
        const T& operator[](ULong index) const { return data()[index]; }

        T* add(VM& vm, const T& value) {
            if (mArray == nullptr || size() >= capacity()) {
                ULong newCapacity = mArray == nullptr ? 8 : capacity() * 2;
                mArray = mMemoryManager.reallocateArray(vm, mArray, newCapacity);
                if (mArray == nullptr) {
                    return nullptr;
                }
            }

            data()[mSize++] = value;
            return &data()[mSize - 1];
        }

        T* add(VM& vm, T&& value) {
            if (mArray == nullptr || size() >= capacity()) {
                ULong newCapacity = capacity() * 2;
                mArray = mMemoryManager.reallocateArray(vm, mArray, newCapacity);
                if (mArray == nullptr) {
                    return nullptr;
                }
            }

            data()[mSize++] = value;
            return &data()[mSize - 1];
        }

        void clear() {
            mSize = 0;
        }

        Iterator begin() { return data(); }
        Iterator end() { return data() + mSize; }

    private:
        gc::MemoryManager& mMemoryManager;
        Object*& mArray;
        ULong& mSize;

        T* data() const {
            return reinterpret_cast<T*>(mArray->asArray()->elementBytes);
        }
    };
}

#endif // BIBBLEVM_CORE_OOP_ARRAY_VIEW_H
