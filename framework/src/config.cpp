// Copyright 2026 JesusTouchMe

#include "BibbleVM/config.h"

namespace bibblevm {
    size_t MemoryConfig::CalculateOldGenHeapMaxSize() {
        return 4ull * 1024 * 1024 * 1024; // TODO: implement memory queries in libos
    }
}
