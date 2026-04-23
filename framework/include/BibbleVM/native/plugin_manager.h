// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_NATIVE_PLUGIN_MANAGER_H
#define BIBBLEVM_PLUGIN_MANAGER_H 1

#include "BibbleVM/core/executor/function.h"

#include "BibbleVM/api.h"

#include <libos/dynlib.h>

#include <algorithm>
#include <vector>

namespace bibblevm::native {
    class BIBBLEVM_EXPORT PluginManager {
    public:
        ~PluginManager();

        bool load(String path);
        bool load(const char* path);

        template<class T>
        T getNativeFunction(std::string_view moduleName, std::string_view functionName) const {
            std::string name = "Bibble/";
            name += moduleName;
            name += "/";
            name += functionName;

            auto replace = [](std::string str, std::string_view value, std::string_view replacement) {
                size_t pos = 0;
                while ((pos = str.find(value, pos)) != std::string::npos) {
                    str.replace(pos, value.length(), replacement);
                    pos += replacement.length();
                }
            };

            replace(name, "_", "_1");
            replace(name, ".", "_2");
            replace(name, "::", "_3");
            replace(name, "$", "_4");

            std::ranges::replace(name, '/', '_');

            return static_cast<T>(getSymbol0(name.c_str()));
        }

    private:
        std::vector<os_dynlib*> mPlugins;

        void* getSymbol0(os_cstring name) const;
    };
}

#endif //BIBBLEVM_NATIVE_PLUGIN_MANAGER_H