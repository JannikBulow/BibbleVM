// Copyright 2026 JesusTouchMe

#include "BibbleVM/module/module.h"

namespace bibblevm::module {
    Module::Module(const String& name, FunctionList functions)
        : mName(name)
        , mFunctions(std::move(functions)) {}
}
