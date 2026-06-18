// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_ALLOCATOR_CODE_ALLOC_H
#define BIBBLEVM_ALLOCATOR_CODE_ALLOC_H 1

#include "BibbleVM/compiler/code.h"

#include "BibbleVM/api.h"

#include <vector>

namespace bibblevm {
    class BIBBLEVM_EXPORT CodeAllocator {
    public:
        ~CodeAllocator();

        // allocate readwrite memory to write code into
        compiler::Code* allocate(size_t size);

        // deallocate code
        void deallocate(compiler::Code* code);

        // mark the code as executable and remove write permissions, then flush it into instruction cache
        void markExecutable(compiler::Code* code);

        // mark the code as read-write and remove executable permissions
        void markReadWrite(compiler::Code* code);

    private:
        std::vector<compiler::Code*> mBuffers;
    };
}

#endif // BIBBLEVM_ALLOCATOR_CODE_ALLOC_H
