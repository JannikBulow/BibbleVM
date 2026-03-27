// Copyright 2026 JesusTouchMe

#include "BibbleVM/module/const_pool.h"

namespace bibblevm::module {
    ConstPool::ConstPool(List<Value> entries)
        : mEntries(std::move(entries)) {}

    Byte ConstPool::getByte(ConstantIndex index) const {
        return mEntries[index].u.b;
    }

    Short ConstPool::getShort(ConstantIndex index) const {
        return mEntries[index].u.s;
    }

    Int ConstPool::getInt(ConstantIndex index) const {
        return mEntries[index].u.i;
    }

    Long ConstPool::getLong(ConstantIndex index) const {
        return mEntries[index].u.l;
    }

    std::string_view ConstPool::getString(ConstantIndex index) const {
        return mEntries[index].u.str;
    }

    ConstPool::ModuleInfo ConstPool::getModuleInfo(ConstantIndex index) const {
        return mEntries[index].u.mi;
    }

    ConstPool::FunctionInfo ConstPool::getFunctionInfo(ConstantIndex index) const {
        return mEntries[index].u.fi;
    }

}
