// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_LINKER_LINKER_H
#define BIBBLEVM_LINKER_LINKER_H 1

#include "BibbleVM/linker/module.h"

namespace bibblevm::linker {
    BIBBLEVM_EXPORT bool ReadModule(VM& vm, Module& module, const char* filePath);
    BIBBLEVM_EXPORT bool ReadModuleFromMemory(VM& vm, Module& module, bibblebytecode::ByteBuffer buffer);
    BIBBLEVM_EXPORT bool ParseModule(VM& vm, Module& module);
    BIBBLEVM_EXPORT bool PreverifyModule(VM& vm, Module& module);
    BIBBLEVM_EXPORT bool LinkModule(VM& vm, Module& module);
    BIBBLEVM_EXPORT bool VerifyModule(VM& vm, Module& module);

    // Full AIO load
    BIBBLEVM_EXPORT bool LoadModule(VM& vm, Module& module, const char* filePath);
}

#endif // BIBBLEVM_LINKER_LINKER_H
