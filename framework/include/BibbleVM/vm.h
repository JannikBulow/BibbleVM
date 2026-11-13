// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_VM_H
#define BIBBLEVM_VM_H 1

#include "BibbleVM/allocator/code_alloc.h"

#include "BibbleVM/compiler/jit/compiler.h"

#include "BibbleVM/core/executor/scheduler.h"

#include "BibbleVM/core/gc/memory_manager.h"

#include "BibbleVM/linker/linker.h"

#include "BibbleVM/native/plugin_manager.h"

#include "BibbleVM/util/time_manager.h"

#include "BibbleVM/api.h"
#include "BibbleVM/config.h"

#include <BibbleInterface.h>

#include <format>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

namespace bibblevm {
    // BibbleVM shared state. BibbleVM intentionally doesn't use a global state so you can have more than one VM running in the same process.
    class BIBBLEVM_EXPORT VM {
    public:
        explicit VM(const Config& config);

        VM(const VM&) = delete;
        VM& operator=(const VM&) = delete;

        VM(VM&&) noexcept = default;
        VM& operator=(VM&&) noexcept = default;

        ~VM();

        operator BibbleVM*() {
            return reinterpret_cast<BibbleVM*>(this);
        }

        const Config& config() const { return mConfig; }

        StringPool& stringPool() { return mStringPool; }
        TimeManager<> timeManager() const { return mTimeManager; }
        native::PluginManager& pluginManager() { return mPluginManager; }
        gc::MemoryManager& memoryManager() { return mMemoryManager; }
        executor::Scheduler& scheduler() { return mScheduler; }
        linker::Linker& linker() { return mLinker; }
        CodeAllocator& codeAllocator() { return mCodeAllocator; }

        String describeOSResult(os_result res);

        template<class... Args>
        constexpr void debugLog(std::string_view system, std::format_string<Args...> fmt, Args&&... args) {
            if (!mConfig.debug.enableDebugLogging) [[likely]] return;

            // TODO: write logs in a file to not mess with the programs stdio
            std::cout << '[' << system << "] " << std::format(fmt, std::forward<Args>(args)...) << std::endl;
        }

        linker::Module* getModule(String name);
        linker::Module* getModule(std::string_view name);

        String getProperty(std::string_view name) const;
        void setProperty(String name, String value);

        // stops the scheduler and kills all threads, ensuring only the caller thread remains to execute exit code, then runs all exit code ensuring everything is fully gone
        // does not exit the process, so it's safe to use with an embedded vm.
        // some memory may still be allocated with the vm after this function returns. this memory is held by the vm object itself and will be deallocated with the destructor
        template<typename Finalizer = std::nullptr_t>
        [[noreturn]] void exit(Finalizer finalizer = nullptr) {
            if (mExited) return;
            mExited = true;

            mScheduler.exit(*this, [finalizer = std::move(finalizer)](VM& vm) {
                vm.mMemoryManager.destroyAllObjects(vm);

                if (vm.mJitCompiler) {
                    vm.mJitCompiler.reset();
                }

                if constexpr (!std::is_same_v<Finalizer, std::nullptr_t>) {
                    finalizer();
                }
            });
        }

        void enqueueForJitCompile(executor::Function* function);

    private:
        Config mConfig;

        StringPool mStringPool;
        TimeManager<> mTimeManager;
        native::PluginManager mPluginManager;
        gc::MemoryManager mMemoryManager;
        executor::Scheduler mScheduler;
        linker::Linker mLinker;

        CodeAllocator mCodeAllocator;

        std::unordered_map<String, oop::Object**, String::Hash, String::Eq> mProperties;

        bool mExited = false;

        std::unique_ptr<jit::Compiler> mJitCompiler;
    };
}

#endif // BIBBLEVM_VM_H
