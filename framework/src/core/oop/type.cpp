// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/core/oop/class.h"
#include "BibbleVM/core/oop/type.h"

namespace bibblevm::oop {
    uint16_t Type::getObjectFieldCount() const {
        return instanceClass->getObjectFieldCount();
    }
}
