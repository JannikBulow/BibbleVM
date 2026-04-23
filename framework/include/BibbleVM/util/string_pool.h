// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_STRING_POOL_H
#define BIBBLEVM_STRING_POOL_H 1

#include "BibbleVM/core/oop/string.h"

#include "BibbleVM/api.h"

#include <unordered_set>

namespace bibblevm {
    using namespace std::string_view_literals;
    using String = oop::String;

    class BIBBLEVM_EXPORT StringPool {
    public:
        // If length == 0 this function calls strlen
        String intern(VM& vm, const char* data, size_t length = 0);
        String intern(VM& vm, std::string_view string);

    private:
        std::unordered_set<String, String::Hash, String::Eq> mStrings;
    };
}

#endif // BIBBLEVM_STRING_POOL_H
