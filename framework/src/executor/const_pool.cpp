// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/executor/const_pool.h"

#include "BibbleVM/util/string_pool.h"

namespace bibblevm::executor {
    ConstPool::ConstPool(uint16_t entryCount, Value* entries)
        : mEntryCount(entryCount)
        , mEntries(entries) {}

    std::optional<ConstPool> ConstPool::merge(const ConstPool& other, GrowingArenaAllocator& allocator) const {
        size_t newEntryCount = mEntryCount + other.mEntryCount;
        if (newEntryCount > 0xFFFF) {
            // erm
            return std::nullopt;
        }

        auto narrowNewEntryCount = static_cast<uint16_t>(newEntryCount);

        auto* newEntries = allocator.allocate<Value>(narrowNewEntryCount);
        if (newEntries == nullptr) {
            // oom error?
            return std::nullopt;
        }

        memcpy(newEntries, mEntries, mEntryCount * sizeof(Value));
        memcpy(newEntries + mEntryCount, other.mEntries, other.mEntryCount * sizeof(Value));

        return ConstPool(narrowNewEntryCount, newEntries);
    }

    Value& ConstPool::get(ConstantIndex index) const {
        return mEntries[index]; // verifier guarantees in bounds :fire:
    }
}
