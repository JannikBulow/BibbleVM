// Copyright 2026 JesusTouchMe

#ifndef BIBBLEVM_MODULE_CONST_POOL_H
#define BIBBLEVM_MODULE_CONST_POOL_H 1

#include "BibbleVM/core/value.h"

#include "BibbleVM/util/list.h"

#include "BibbleVM/api.h"

#include <memory>
#include <string_view>

namespace bibblevm::module {
    using ConstantIndex = uint16_t;

    class BIBBLEVM_EXPORT ConstPool {
    public:
        enum Tag : uint8_t {
            BYTE,
            SHORT,
            INT,
            LONG,
            STRING,
            MODULE_INFO,
            FUNCTION_INFO,
        };

        struct ModuleInfo {
            ConstantIndex name;
        };

        struct FunctionInfo {
            ConstantIndex module;
            ConstantIndex name;
        };

        struct Value {
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

        explicit ConstPool(List<Value> entries);

        Byte getByte(ConstantIndex index) const;
        Short getShort(ConstantIndex index) const;
        Int getInt(ConstantIndex index) const;
        Long getLong(ConstantIndex index) const;
        std::string_view getString(ConstantIndex index) const;
        ModuleInfo getModuleInfo(ConstantIndex index) const;
        FunctionInfo getFunctionInfo(ConstantIndex index) const;

    private:
        List<Value> mEntries;
    };
}

#endif //BIBBLEVM_MODULE_CONST_POOL_H