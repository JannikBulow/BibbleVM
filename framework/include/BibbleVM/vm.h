// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_VM_H
#define BIBBLEVM_VM_H 1

#include "BibbleVM/module/module.h"

#include "BibbleVM/api.h"

namespace bibblevm {
    // BibbleVM shared state. BibbleVM intentionally doesn't use a global state so you can have more than one VM running on your system.
    class BIBBLEVM_EXPORT VM {
    public:
        VM() = default;

        VM(const VM&) = delete;
        VM& operator=(const VM&) = delete;

        VM(VM&&) noexcept = default;
        VM& operator=(VM&&) noexcept = default;

        const ModuleList& getModules() const { return mModules; }

        void addModule(ModulePtr module);

        Module* getModule(std::string_view name) const;
        Module* getModule(String name) const;

        Module* getEntryPointModule() const;
        Function* getEntryPointFunction(Module* module) const;

    private:
        ModuleList mModules;
    };
}

#endif // BIBBLEVM_VM_H
