// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/native/interface.h"

#include "BibbleVM/vm.h"

namespace bibblevm::native {
    void Test(BibbleVM* vm_) {
        VM& vm = *reinterpret_cast<VM*>(vm_); // :(
        vm.debugLog("native-interface", "Test");
    }

    void PopulateInterface(BibbleInterface* interface) {
        interface->Test = Test;
    }
}
