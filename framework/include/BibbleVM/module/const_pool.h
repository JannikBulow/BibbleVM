// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_MODULE_CONST_POOL_H
#define BIBBLEVM_MODULE_CONST_POOL_H 1

#include "BibbleVM/core/value.h"

#include "BibbleVM/api.h"

#include <memory>
#include <string_view>

namespace bibblevm::module {
    using ConstantIndex = uint16_t;

    class ConstPool {
    public:
        enum Tag : uint8_t {
            BYTE = 0x01,
            SHORT = 0x02,
            INT = 0x03,
            LONG = 0x04,
            STRING = 0x05,
            MODULE_INFO = 0x06,
            FUNCTION_INFO = 0x07,
        };

        struct ModuleInfo {
            ConstantIndex name;
        };

        struct FunctionInfo {
            ConstantIndex module;
            ConstantIndex name;
        };

        struct Entry {
            Tag tag;
            union {
                Byte b;
                Short s;
                Int i;
                Long l;
                std::string_view str;
                ModuleInfo mi;
                FunctionInfo fi;
            } u;
        };

        ConstPool() = default;

        ConstPool(uint16_t entryCount, Entry* entries)
            : mEntryCount(entryCount)
            , mEntries(entries) {}

        uint16_t getEntryCount() const { return mEntryCount; }
        Entry* getEntries() const { return mEntries; }

    private:
        uint16_t mEntryCount;
        Entry* mEntries;
    };
}

#endif //BIBBLEVM_MODULE_CONST_POOL_H