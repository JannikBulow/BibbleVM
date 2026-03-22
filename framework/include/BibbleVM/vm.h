// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_VM_H
#define BIBBLEVM_VM_H 1

#include "BibbleVM/api.h"

namespace bibblevm {
    // BibbleVM shared state. BibbleVM intentionally doesn't use a global state so you can have more than one VM running on your system.
    class BIBBLEVM_EXPORT VM {
    public:
    };
}

#endif // BIBBLEVM_VM_H
