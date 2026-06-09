// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_LINKER_INTRINSICS_H
#define BIBBLEVM_LINKER_INTRINSICS_H 1

#include "BibbleVM/core/executor/function.h"

namespace bibblevm::linker {
    struct IntrinsicFunction {
        executor::EntryPoint entryPoint;
    };

    struct IntrinsicModule {
    };

    const IntrinsicModule* GetIntrinsicsModule(std::string_view name);
    const IntrinsicFunction* GetIntrinsicsFunction(const IntrinsicModule* module, std::string_view name);
}

#endif // BIBBLEVM_LINKER_INTRINSICS_H
