// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_LINKER_LINKER_H
#define BIBBLEVM_LINKER_LINKER_H 1

#include "BibbleVM/linker/module.h"

#include "BibbleVM/util/string_pool.h"

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace bibblevm::linker {
    class BIBBLEVM_EXPORT Linker {
    public:
        void addModulePath(std::string path);
        void addModulePath(std::vector<std::string> path);

        Module* loadModule(VM& vm, String name);
        Module* loadModule(VM& vm, std::string_view name);
        Module* loadModule(VM& vm, String name, bibblebytecode::ReadableByteBuffer memory);
        Module* loadModule(VM& vm, std::string_view name, bibblebytecode::ReadableByteBuffer memory);

    private:
        std::vector<std::string> mModulePath;
        std::vector<std::unique_ptr<Module>> mModules;

        bool readModuleFromPath(VM& vm, Module& module, std::string_view path, std::string_view name);
        bool readModuleFromMemory(VM& vm, Module& module, bibblebytecode::ReadableByteBuffer& memory);
        bool parseModule(VM& vm, Module& module);
        bool preverifyModule(VM& vm, Module& module);
        bool linkModule(VM& vm, Module& module);
        bool verifyModule(VM& vm, Module& module);
    };
}

#endif // BIBBLEVM_LINKER_LINKER_H
