// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/core/executor/module.h"

namespace bibblevm::executor {
    Module::Module(String name, ConstPool constPool, uint16_t classCount, oop::Class* classes, uint16_t functionCount, Function* functions)
        : mName(name)
        , mConstPool(std::move(constPool))
        , mClassCount(classCount)
        , mClasses(classes)
        , mFunctionCount(functionCount)
        , mFunctions(functions) {}

    oop::Class* Module::getClass(String name) const {
        for (uint16_t i = 0; i < mClassCount; i++) {
            oop::Class& clas = mClasses[i];
            if (clas.getName() == name) return &clas;
        }
        return nullptr;
    }

    oop::Class* Module::getClass(std::string_view name) const {
        for (uint16_t i = 0; i < mClassCount; i++) {
            oop::Class& clas = mClasses[i];
            if (clas.getName() == name) return &clas;
        }
        return nullptr;
    }

    Function* Module::getFunction(String name) const {
        for (uint16_t i = 0; i < mFunctionCount; i++) {
            Function& function = mFunctions[i];
            if (function.getName() == name) return &function;
        }
        return nullptr;
    }

    Function* Module::getFunction(std::string_view name) const {
        for (uint16_t i = 0; i < mFunctionCount; i++) {
            Function& function = mFunctions[i];
            if (function.getName() == name) return &function;
        }
        return nullptr;
    }
}
