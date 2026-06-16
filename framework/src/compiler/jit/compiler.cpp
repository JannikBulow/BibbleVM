// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/compiler/jit/compiler.h"

#include "BibbleVM/compiler/compiler.h"

namespace bibblevm::jit {
    Compiler::Compiler(VM& vm) {
        spawnCompilerThread(vm);
    }

    Compiler::~Compiler() {
        {
            std::lock_guard lock(mMutex);
            mStop = true;
        }

        mCondition.notify_all();

        for (auto& thread : mThreads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    void Compiler::spawnCompilerThread(VM& vm) {
        mThreads.emplace_back([this] (VM* vm) {
            workerLoop(*vm);
        }, &vm);
    }

    void Compiler::enqueueForCompilation(executor::Function* function) {
        std::lock_guard lock(mMutex);
        mCompileQueue.push(function);
        mCondition.notify_one();
    }

    void Compiler::workerLoop(VM& vm) {
        try {
            while (true) {
                executor::Function* function = nullptr;

                {
                    std::unique_lock lock(mMutex);

                    mCondition.wait(lock, [this] {
                        return mStop || !mCompileQueue.empty();
                    });

                    if (mStop) [[unlikely]] {
                        return;
                    }

                    function = mCompileQueue.front();
                    mCompileQueue.pop();
                }

                compileOne(vm, function);
            }
        } catch (...) {
            spawnCompilerThread(vm); // restart the thread lol
        }
    }

    void Compiler::compileOne(VM& vm, executor::Function* function) {
        CompilationUnit* unit = new CompilationUnit();
        function->setCompilationUnit(unit);

        unit->state = CompilationState::Compiling;

        CompileOptions options;
        options.opt = OptLevel::O0;

        ::bibblevm::Compiler compiler;
        Code* baseline = compiler.compile(vm, function, options);

        unit->baseline = baseline;
        unit->state = CompilationState::Compiled;

        function->setEntryPoint((executor::EntryPoint) baseline->mc);
    }
}
