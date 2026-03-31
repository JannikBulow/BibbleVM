// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_VM_H
#define BIBBLEVM_VM_H 1

#include "BibbleVM/executor/scheduler.h"

#include "BibbleVM/linker/module.h"

#include "BibbleVM/api.h"

#include <memory>
#include <vector>

namespace bibblevm {
    // BibbleVM shared state. BibbleVM intentionally doesn't use a global state so you can have more than one VM running in the same process.
    class BIBBLEVM_EXPORT VM {
    public:
        VM() = default;

        VM(const VM&) = delete;
        VM& operator=(const VM&) = delete;

        VM(VM&&) noexcept = default;
        VM& operator=(VM&&) noexcept = default;

        StringPool& stringPool() { return mStringPool; }
        executor::Scheduler& scheduler() { return mScheduler; }

        linker::Module* getModule(String name) const;
        linker::Module* getModule(std::string_view name) const;

        void addModule(std::unique_ptr<linker::Module> module);

    private:
        StringPool mStringPool;
        executor::Scheduler mScheduler;

        std::vector<std::unique_ptr<linker::Module>> mModules; // TODO: make a better solution for the memory shit here
    };
}

#endif // BIBBLEVM_VM_H
