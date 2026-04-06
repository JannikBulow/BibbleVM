// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/core/executor/module.h"

namespace bibblevm::executor {
    Module::Module(String name, ConstPool constPool, uint16_t functionCount, Function* functions)
        : mName(name)
        , mConstPool(std::move(constPool))
        , mFunctionCount(functionCount)
        , mFunctions(functions) {}

    Function* Module::getFunction(String name) const {
        for (uint16_t i = 0; i < mFunctionCount; ++i) {
            Function& function = mFunctions[i];
            if (function.getName() == name) return &function;
        }
        return nullptr;
    }

    Function* Module::getFunction(std::string_view name) const {
        for (uint16_t i = 0; i < mFunctionCount; ++i) {
            Function& function = mFunctions[i];
            if (function.getName() == name) return &function;
        }
        return nullptr;
    }
}
