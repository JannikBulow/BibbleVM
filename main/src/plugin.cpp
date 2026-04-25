// Copyright 2026 Jannik Laugmand Bülow

#include <BibbleInterface.h>

BIBBLE_EXPORT BIBBLE_CALL VMReturnValue Bibble_Main_test(BibbleVM* vm, BibbleInterface* interface, VMValue* args) {
    VMModule intrinsics = interface->GetModule(vm, "Intrinsics");
    VMFunction print = interface->GetFunction(vm, intrinsics, "print");
    (void) interface->CallFunction(vm, interface, print, BIBBLE_ARG(VMLong, 1234));

    return BIBBLE_RETURN(VMByte, 69);
}