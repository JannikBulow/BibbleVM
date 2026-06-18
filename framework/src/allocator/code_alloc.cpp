// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/allocator/code_alloc.h"

#include <libos/memory.h>

namespace bibblevm {
    CodeAllocator::~CodeAllocator() {
        for (compiler::Code* code : mBuffers) {
            deallocate(code);
        }
    }

    compiler::Code* CodeAllocator::allocate(size_t size) {
        void* mc;
        os_result res = os_mem_allocate(&mc, nullptr, size, OS_MEM_RESERVE | OS_MEM_COMMIT, OS_MEM_PROTECT_READWRITE);
        if (res != OS_OK) {
            return nullptr;
        }

        compiler::Code* code = new compiler::Code(mc, size);
        mBuffers.push_back(code);

        return code;
    }

    void CodeAllocator::deallocate(compiler::Code* code) {
        if (code == nullptr) [[unlikely]] return;

        os_result res = os_mem_free(code->mc, code->size, OS_MEM_RELEASE);
        if (res != OS_OK) {
            //TODO: warning maybe. just debug break on guy rn
            int guy = 55;
        }

        for (size_t i = 0; i < mBuffers.size(); ++i) {
            if (mBuffers[i] == code) {
                if (i == mBuffers.size() - 1) {
                    mBuffers.pop_back();
                } else {
                    mBuffers[i] = mBuffers.back();
                    mBuffers.pop_back();
                }
                break;
            }
        }

        delete code;
    }

    void CodeAllocator::markExecutable(compiler::Code* code) {
        if (code == nullptr) [[unlikely]] return;

        os_result res = os_mem_protect(code->mc, code->size, OS_MEM_PROTECT_EXECUTE_READ);
        if (res != OS_OK) {
            return;
        }
    }

    void CodeAllocator::markReadWrite(compiler::Code* code) {
        if (code == nullptr) [[unlikely]] return;

        os_result res = os_mem_protect(code->mc, code->size, OS_MEM_PROTECT_READWRITE);
        if (res != OS_OK) {
            return;
        }
    }
}
