// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/core/executor/const_pool.h"

#include "BibbleVM/util/string_pool.h"

namespace bibblevm::executor {
    ConstPool::ConstPool(uint16_t entryCount, Value* entries)
        : mEntryCount(entryCount)
        , mEntries(entries) {}

    uint16_t ConstPool::getEntryCount() const {
        return mEntryCount;
    }

    Value& ConstPool::get(ConstantIndex index) const {
        return mEntries[index]; // verifier guarantees in bounds :fire:
    }
}
