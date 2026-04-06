// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_CORE_MODULE_FUNCTION_H
#define BIBBLEVM_CORE_MODULE_FUNCTION_H 1

#include "BibbleVM/core/module/const_pool.h"

#include "BibbleVM/api.h"

#include <cstdint>

namespace bibblevm::module {
    enum FunctionFlags : uint16_t {
        FUNC_NATIVE = 0x0001,
    };

    struct Function {
        ConstantIndex name;
        uint16_t flags;
        uint16_t registerCount;
        uint16_t parameterCount;
        ConstPool constPool;
        uint32_t bytecodeSize;
        const uint8_t* bytecode;
    };
}

#endif // BIBBLEVM_CORE_MODULE_FUNCTION_H
