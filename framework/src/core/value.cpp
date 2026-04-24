// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/core/gc/memory_manager.h"

#include "BibbleVM/core/value.h"

namespace bibblevm {
    VMValue Value::toNative(gc::MemoryManager& memoryManager) const {
        VMValue out{};
        if (isObject) {
            out.obj = reinterpret_cast<VMObject>(memoryManager.newLocalStrongReference(obj));
        } else {
            out.b = b;
            out.ub = ub;
            out.s = s;
            out.us = us;
            out.i = i;
            out.ui = ui;
            out.l = l;
            out.ul = ul;
            out.f = f;
            out.d = d;
            out.h = h;
        }
        return out;
    }
}
