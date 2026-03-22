// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_INTERPRETER_H
#define BIBBLEVM_INTERPRETER_H 1

#include "BibbleVM/api.h"

namespace bibblevm {
    class VM;

    using Interpreter = bool(*)(VM& vm);
}

#endif // BIBBLEVM_INTERPRETER_H
