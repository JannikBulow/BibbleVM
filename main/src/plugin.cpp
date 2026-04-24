// Copyright 2026 Jannik Laugmand Bülow

#include <BibbleInterface.h>

BIBBLE_EXPORT BIBBLE_CALL VMReturnValue Bibble_Main_test(BibbleVM* vm, BibbleInterface* interface, VMValue* args) {
    VMModule intrinsics = interface->GetModule(vm, "Intrinsics");

    return {false, {.l = 69}};
}