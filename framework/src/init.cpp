// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/init.h"

#include <libos/init.h>

namespace bibblevm {
    void InitDependencies() {
        libos_init();
    }
}
