// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_LINKER_INTRINSICS_H
#define BIBBLEVM_LINKER_INTRINSICS_H 1

#include "BibbleVM/executor/function.h"

namespace bibblevm::linker {
    struct IntrinsicFunction {
        std::string_view name;
        executor::EntryPoint entryPoint;
    };

    struct IntrinsicModule {
        std::string_view name;
        const IntrinsicFunction* functions;
        size_t functionCount;
    };

    const IntrinsicModule* GetIntrinsicsModule(std::string_view name);
    const IntrinsicFunction* GetIntrinsicsFunction(const IntrinsicModule* module, std::string_view name);
}

#endif // BIBBLEVM_LINKER_INTRINSICS_H
