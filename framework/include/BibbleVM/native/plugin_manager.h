// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_NATIVE_PLUGIN_MANAGER_H
#define BIBBLEVM_PLUGIN_MANAGER_H 1

#include "BibbleVM/core/executor/function.h"

#include "BibbleVM/api.h"

#include <libos/dynlib.h>

#include <vector>

namespace bibblevm::native {
    class BIBBLEVM_EXPORT PluginManager {
    public:
        ~PluginManager();

        bool load(String path);

        template<class T>
        T getNativeFunction(std::string_view moduleName, std::string_view functionName) const {
            std::string name;
            name += moduleName;
            name += functionName;
            return static_cast<T>(getSymbol0(name.c_str()));
        }

    private:
        std::vector<os_dynlib*> mPlugins;

        void* getSymbol0(os_cstring name) const;
    };
}

#endif //BIBBLEVM_NATIVE_PLUGIN_MANAGER_H