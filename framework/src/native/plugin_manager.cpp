// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/native/plugin_manager.h"

namespace bibblevm::native {
    PluginManager::~PluginManager() {
        for (os_dynlib* plugin : mPlugins) {
            os_dynlib_unload(plugin);
        }
    }

    bool PluginManager::load(String path) {
        return load(path.getData());
    }

    bool PluginManager::load(const char* path) {
        os_dynlib* plugin;
        os_result res = os_dynlib_load(&plugin, path, OS_DYNLIB_EAGER | OS_DYNLIB_LOCAL);
        if (res != OS_OK) {
            if (res > 0) os_dynlib_unload(plugin);
            return false;
        }

        mPlugins.push_back(plugin);
        return true;
    }

    void* PluginManager::getSymbol0(os_cstring name) const {
        for (os_dynlib* plugin : mPlugins) {
            void* symbol;
            os_result res = os_dynlib_get_symbol(plugin, name, &symbol);
            if (res == OS_OK && symbol != nullptr) return symbol;
        }

        return nullptr;
    }
}
