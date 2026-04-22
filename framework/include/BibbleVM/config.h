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
        double nurseryGrowthFactor = 4.0;
        double nurseryGrowthThreshold = 0.6; // how full the nursery can be (after collection) before resize happens. nursery resize is a process that can't be done in stages so it will ignore the pause budget. keep this and growth factor high to minimize resizes

        size_t oldGenHeapMinSize = 64 * 1024 * 1024;
        size_t oldGenHeapMaxSize = CalculateOldGenHeapMaxSize();
        size_t oldGenHeapRegionSize = 4 * 1024 * 1024;
        double oldGenHeapGrowthFactor = 2.0;

        size_t largeObjectThreshold = 256 * 1024; // When is an object considered too big to be copied or moved during gc? Large objects are never moved due to efficiency loss

        uint8_t promotionAge = 3;

        bool implicitInternStrings = false; // all string allocations are implicitly interned in the string pool. very inefficient speed wise but less memory consumption

        bool enableLargeHeap = true; // enable whole separate heap for large object allocation (instead of using the old generation heap)

    private:
        static size_t CalculateOldGenHeapMaxSize();
    };

    struct GCConfig {
        Nanoseconds pauseBudget = 200ms;

        size_t rememberedSetReserve = 1024;

        double nurseryCollectionThreshold = 0.95;
        double oldGenHeapCollectionThreshold = 0.7;
        double oldGenHeapCompactThreshold = 0.7; // how fragmented should the heap be before compaction
        double promotionPressureThreshold = 0.15;

        uint8_t promotionAge = 3;

        // For advanced users only. Should NEVER be enabled in programs that utilize the Bibble Standard Library
        bool disableFinalizers = false;

        // This will be implemented in 2055
        bool enableConcurrentGC = false;
    };

    struct SchedulerConfig {
        size_t maxTasks = SIZE_MAX; // The scheduler will throw an exception and shutdown the whole VM if this gets exceeded, so use with caution.

        size_t maxStackSize = 4 * 1024 * 1024;

        struct {
            bool enabled = true;
            size_t threshold = 1000; // Anti bad code measure. Will auto yield after this many instructions have executed to allow the scheduler to run other tasks.
        } autoYielding;

        struct {
            uint8_t levels = 1;
            uint8_t boost = 1;
        } priority;
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