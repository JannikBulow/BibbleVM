// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_EXECUTOR_CONST_POOL_H
#define BIBBLEVM_EXECUTOR_CONST_POOL_H 1

#include "BibbleVM/allocator/arena.h"

#include "BibbleVM/module/const_pool.h"

#include "BibbleVM/util/string_pool.h"

#include "BibbleVM/api.h"

#include <optional>

namespace bibblevm::executor {
    class Function;
    class Module;

    using ConstantIndex = module::ConstantIndex;

    class BIBBLEVM_EXPORT ConstPool {
    public:
        ConstPool() = default;
        ConstPool(uint16_t entryCount, Value* entries);

        ConstPool(const ConstPool&) = delete; // fuh no copies
        ConstPool(ConstPool&&) = default;
        ConstPool& operator=(const ConstPool&) = delete;
        ConstPool& operator=(ConstPool&&) = default;

        // Constructs a new ConstPool with `this` as the first elements and `other` after
        std::optional<ConstPool> merge(const ConstPool& other, GrowingArenaAllocator& allocator) const; // TODO: generic allocator solution maybe

        Value& get(ConstantIndex index) const;

    private:
        uint16_t mEntryCount;
        Value* mEntries;
    };
}

#endif // BIBBLEVM_EXECUTOR_CONST_POOL_H
