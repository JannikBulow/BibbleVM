// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_FUNCTION_H
#define BIBBLEVM_FUNCTION_H 1

#include "BibbleVM/executor/instruction.h"

#include "BibbleVM/api.h"

namespace bibblevm {
    class BIBBLEVM_EXPORT Function {
    public:
        Function(InstructionList instructions);
    };
}

#endif // BIBBLEVM_FUNCTION_H
