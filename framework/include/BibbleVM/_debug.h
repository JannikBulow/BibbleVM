// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_DEBUG_H
#define BIBBLEVM_DEBUG_H 1

#include <iostream>

#ifdef BIBBLEVM_ENABLE_DEBUG
namespace bibblevm {
    inline void _assert_fail(const char* expr, const char* file, int line) {
        std::cout << "BibbleVM: ASSERT FAIL: " << expr << " at " << file << ":" << line << std::endl;
        std::exit(5);
    }
}
#define BIBBLEVM_ASSERT(expr) if (!(expr)) ::bibblevm::_assert_fail(#expr, __FILE__, __LINE__)
#else
#define BIBBLEVM_ASSERT(expr)
#endif

#endif //BIBBLEVM_DEBUG_H
