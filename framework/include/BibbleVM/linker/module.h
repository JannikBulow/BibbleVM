// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_LINKER_MODULE_H
#define BIBBLEVM_LINKER_MODULE_H 1

#include "BibbleVM/allocator/arena.h"

#include "BibbleVM/core/executor/module.h"

#include "BibbleVM/core/module/module.h"

#include <condition_variable>
#include <mutex>

namespace bibblevm::linker {
    enum class Stage {
        None = 0,
        FileLoaded = 1,
        Parsed = 2,
        PreVerified = 3,
        Linked = 4,
        Verified = 5,
        Ready = 6
    };

    class Module {
    public:
        GrowingArenaAllocator& arena() { return mArena; }
        module::Module& rawModule() { return mRawModule; }
        executor::Module& linkedModule() { return mLinkedModule; }

        Stage getStage() const {
            std::lock_guard lock(mMutex);
            return mStage;
        }

        void setStage(Stage stage) {
            std::lock_guard lock(mMutex);
            mStage = stage;
            mCondition.notify_all();
        }

        void waitForStage(Stage stage) const {
            std::unique_lock lock(mMutex);
            if (mStage >= stage) return;
            mCondition.wait(lock, [this, stage] { return mStage >= stage; });
        }

    private:
        GrowingArenaAllocator mArena = GrowingArenaAllocator::Create(64 * 1024);

        Stage mStage = Stage::None;

        module::Module mRawModule{};
        executor::Module mLinkedModule{};

        mutable std::mutex mMutex;
        mutable std::condition_variable mCondition;
    };
}

#endif // BIBBLEVM_LINKER_MODULE_H
