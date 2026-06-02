// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/native/interface.h"

#include "BibbleVM/vm.h"

namespace bibblevm {
    VM::VM(const Config& config)
        : mConfig(config)
        , mScheduler(*this) {
        mMemoryManager.init(*this); // TODO: check result and throw a tantrum
    }

    String VM::describeOSResult(os_result res) {
        os_string description = os_result_describe(res);
        String string = mMemoryManager.allocateString(*this, description)->asString();
        os_free_message(description);
        return string;
    }

    linker::Module* VM::getModule(String name) {
        return mLinker.loadModule(*this, name);
    }

    linker::Module* VM::getModule(std::string_view name) {
        return mLinker.loadModule(*this, name);
    }
}
