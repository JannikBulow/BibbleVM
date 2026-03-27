// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/vm.h"

namespace bibblevm {
    void VM::addModule(ModulePtr module) {
        mModules.push_back(std::move(module));
    }

    Module* VM::getModule(std::string_view name) const {
        for (const auto& module : mModules) {
            if (module->getName() == name) return module.get();
        }
        return nullptr;
    }

    Module* VM::getModule(String name) const {
        for (const auto& module : mModules) {
            if (module->getName() == name) return module.get();
        }
        return nullptr;
    }

    Module* VM::getEntryPointModule() const {
        return getModule("Main");
    }

    Function* VM::getEntryPointFunction(Module* module) const {
        if (module == nullptr) return nullptr;
        return module->getFunction("main");
    }
}
