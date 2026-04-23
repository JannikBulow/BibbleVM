// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_NATIVE_INTERFACE_H
#define BIBBLEVM_NATIVE_INTERFACE_H 1

#include "BibbleVM/api.h"

#include <BibbleInterface.h>

namespace bibblevm::native {
    BIBBLEVM_EXPORT void PopulateInterface(BibbleInterface* interface);
}

#endif //BIBBLEVM_NATIVE_INTERFACE_H