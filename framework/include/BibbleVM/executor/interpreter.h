// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_INTERPRETER_H
#define BIBBLEVM_INTERPRETER_H 1

#include "BibbleVM/api.h"

#include <cstdint>

namespace bibblevm {
    class VM;

    using Interpreter = bool(*)(VM& vm, uint8_t a, uint8_t b, uint8_t c);
}

#endif // BIBBLEVM_INTERPRETER_H
