// Copyright 2026 Jannik Laugmand Bülow

#include <BibbleInterface.h>

BIBBLE_EXPORT BIBBLE_CALL VMReturnValue Bibble_Main_test(BibbleVM* vm, BibbleInterface* interface, VMValue* args) {
    interface->Test(vm);
    return {VM_TYPE_PRIMITIVE, {.l = 69}};
}