// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/native/interface.h"

#include "BibbleVM/vm.h"

namespace bibblevm {
    VM::VM(const Config& config)
        : mConfig(config)
        , mScheduler(*this) {
        mMemoryManager.init(*this); // TODO: check result and throw a tantrum

        if (mConfig.compiler.jit.enabled) mJitCompiler = std::make_unique<jit::Compiler>(*this);
    }

    VM::~VM() {
        this->exit(); // using this-> because exit is also a c function and idk how ts works
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

    String VM::getProperty(std::string_view name) const {
        auto it = mProperties.find(name);
        if (it != mProperties.end()) return (*it->second)->asString();
        return nullptr;
    }

    void VM::setProperty(String name, String value) {
        oop::Object** valueRef = memoryManager().newGlobalStrongReference(value.get()->asObject());
        auto [it, inserted] = mProperties.try_emplace(name, valueRef);

        if (!inserted) {
            memoryManager().deleteGlobalStrongReference(it->second);
            it->second = valueRef;
        }
    }

    // stops the scheduler and kills all threads, ensuring only the caller thread remains to execute exit code, then runs all exit code ensuring everything is fully gone
    // does not exit the process, so it's safe to use with an embedded vm.
    // some memory may still be allocated with the vm after this function returns. this memory is held by the vm object itself and will be deallocated with the destructor
    void VM::exit() {
        if (mExited) return;
        mExited = true;

        mScheduler.exit(*this, [](VM& vm) {
            vm.mMemoryManager.destroyAllObjects(vm);

            if (vm.mJitCompiler) {
                vm.mJitCompiler.reset();
            }
        });
    }

    void VM::enqueueForJitCompile(executor::Function* function) {
        if (mJitCompiler) {
            mJitCompiler->enqueueForCompilation(function);
        }
    }
}
