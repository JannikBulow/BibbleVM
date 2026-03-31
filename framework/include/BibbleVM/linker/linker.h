// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_LINKER_LINKER_H
#define BIBBLEVM_LINKER_LINKER_H 1

#include "BibbleVM/linker/module.h"

namespace bibblevm::linker {
    BIBBLEVM_EXPORT bool LinkModule(VM& vm, Module& module);
}

#endif // BIBBLEVM_LINKER_LINKER_H
