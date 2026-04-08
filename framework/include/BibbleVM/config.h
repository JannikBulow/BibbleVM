// Copyright 2026 JesusTouchMe

#ifndef BIBBLEVM_CONFIG_H
#define BIBBLEVM_CONFIG_H 1

#include "BibbleVM/util/time_manager.h"

#include "BibbleVM/api.h"

#include <cstddef>
#include <cstdint>

namespace bibblevm {
    struct BIBBLEVM_EXPORT MemoryConfig {
        size_t nurseryMinSize = 64 * 1024 * 1024; // per space. these numbers are doubled in actual memory usage
        size_t nurseryMaxSize = 512 * 1024 * 1024;
        double nurseryGrowthFactor = 2.0;

        size_t oldGenHeapMinSize = 64 * 1024 * 1024;
        size_t oldGenHeapMaxSize = CalculateOldGenHeapMaxSize();
        size_t oldGenHeapRegionSize = 4 * 1024 * 1024;
        double oldGenHeapGrowthFactor = 2.0;
        double oldGenHeapCompactThreshold = 0.7;

        size_t largeObjectThreshold = 256 * 1024; // When is an object considered too big to be copied or moved during gc? Large objects are never moved due to efficiency loss

        uint8_t promotionAge = 3;

        bool implicitInternStrings = false; // all string allocations are implicitly interned in the string pool. very inefficient

        bool enableLargeHeap = true; // enable whole separate heap for large object allocation (instead of using the old generation heap)

    private:
        static size_t CalculateOldGenHeapMaxSize();
    };

    struct GCConfig {
        Nanoseconds pauseBudget = 200us;

        size_t rememberedSetReserve = 1024;

        double nurseryCollectionThreshold = 0.95;
        double oldGenHeapCollectionThreshold = 0.7;
        size_t largeObjectCollectionFrequency = 10; // how many gc cycles before we consider scanning the large object heap
        double promotionPressureThreshold = 0.15;

        // For advanced users only. Should NEVER be enabled in programs that utilize the Bibble Standard Library
        bool disableFinalizers = false;

        bool enableConcurrentGC = false;
    };

    struct SchedulerConfig {
        size_t maxTasks = -1; // The scheduler will throw an exception and shutdown the whole VM if this gets exceeded, so use with caution.

        size_t maxStackSize = 4 * 1024 * 1024;

        size_t autoYieldInstructionThreshold = 10000; // Anti bad code measure. Will auto yield after this many instructions have run to allow the scheduler to run other tasks.

        // as great as these two may seem, they waste processing time on scheduling instead of running code. simple scheduler is usually fine
        bool enableTaskPriorities = false;
        bool enableFairScheduling = false;
    };

    struct DebugConfig {
        bool enableDebugLogging = false;

        bool verifyHeapAfterGC = false;
        bool poisonFreedMemory = false;
        bool trackAllocations = false;
        bool logPromotions = false;
        bool logSafepoints = false;
        bool stressGC = false;

        bool verboseSchedulerLogging = false;
    };

    struct Config {
        MemoryConfig memory;
        GCConfig gc;
        SchedulerConfig scheduler;
        DebugConfig debug;
    };
}

#endif //BIBBLEVM_CONFIG_H