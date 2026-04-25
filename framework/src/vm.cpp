// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/native/interface.h"

#include "BibbleVM/vm.h"

namespace bibblevm {
    VM::VM(const Config& config)
        : mConfig(config)
        , mScheduler(*this) {
        mMemoryManager.init(*this); // TODO: check result and throw a tantrum
    }

    linker::Module* VM::getModule(String name) const {
        for (const auto& module : mModules) {
            if (module->linkedModule().getName() == name) return module.get();
        }
        return nullptr;
    }

    linker::Module* VM::getModule(std::string_view name) const {
        for (const auto& module : mModules) {
            if (module->linkedModule().getName() == name) return module.get();
        }
        return nullptr;
    }

    void VM::addModule(std::unique_ptr<linker::Module> module) {
        mModules.push_back(std::move(module));
    }
}
