// Copyright 2026 JesusTouchMe

#ifndef BIBBLEVM_LIST_H
#define BIBBLEVM_LIST_H 1

#include "BibbleVM/util/string_pool.h"

#include "BibbleVM/api.h"

#include <initializer_list>

namespace bibblevm {
    template<class T>
    concept HasGetName = requires(T a) {
        { a.getName() } -> std::convertible_to<String>;
    };

    template<class T>
    class List {
    public:
        struct Iterator {
            T* value;

            Iterator(T* value) : value(value) {}

            Iterator& operator++() {
                ++value;
                return *this;
            }

            Iterator operator++(int) {
                Iterator tmp = *this;
                ++value;
                return tmp;
            }

            T& operator*() {
                return *value;
            }

            bool operator==(const Iterator& other) const {
                return value == other.value;
            }

            bool operator!=(const Iterator& other) const {
                return value != other.value;
            }
        };

        List(std::unique_ptr<T[]> elements, size_t count)
            : mElements(std::move(elements))
            , mCount(count) {}

        template<class... Args>
        List(Args&&... args) {
            mCount = sizeof...(Args);
            mElements = static_cast<T*>(malloc(sizeof(T) * mCount));
            size_t index = 0;
            ((new (&mElements[index++]) T(std::forward<Args>(args))), ...);
        }

        List(const List& other) = delete;

        List(List&& other) noexcept
            : mElements(other.mElements)
            , mCount(other.mCount) {
            other.mElements = nullptr;
            other.mCount = 0;
        }

        Iterator begin() const { return Iterator(mElements); }
        Iterator end() const { return Iterator(mElements + mCount); }

        T* getElements() const { return mElements; }
        size_t getCount() const { return mCount; }

        T* get(size_t index) const {
            if (index < mCount) return mElements[index];
            return nullptr;
        }

        T& operator[](size_t index) const {
            return mElements[index];
        }

        T* getByName(std::string_view name) const requires HasGetName<T> {
            for (auto& element : *this) {
                if (element.getName() == name) {
                    return &element;
                }
            }
            return nullptr;
        }

        T* getByName(String name) const requires HasGetName<T> {
            for (auto& element : *this) {
                if (element.getName() == name) {
                    return &element;
                }
            }
            return nullptr;
        }

    private:
        T* mElements;
        size_t mCount;
    };
}

#endif //BIBBLEVM_LIST_H