// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_COMPILER_JIT_COMPILER_H
#define BIBBLEVM_COMPILER_JIT_COMPILER_H 1

#include "BibbleVM/core/executor/function.h"

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace bibblevm::jit {
    using namespace compiler;

    class BIBBLEVM_EXPORT Compiler {
    public:
        explicit Compiler(VM& vm);
        ~Compiler();

        void spawnCompilerThread(VM& vm); // spawn a thread that accepts compilation requests

        void enqueueForCompilation(executor::Function* function);

    private:
        std::mutex mMutex;
        std::condition_variable mCondition;
        std::queue<executor::Function*> mCompileQueue;

        std::vector<std::thread> mThreads;

        bool mStop = false;

        void workerLoop(VM& vm);
        void compileOne(VM& vm, executor::Function* function);
    };
}

#endif // BIBBLEVM_COMPILER_JIT_COMPILER_H
