// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/util/string_pool.h"

#include <cstring>

namespace bibblevm {
    String StringPool::intern(const char* data, size_t length) {
        if (length == 0) length = std::strlen(data);

        String str(data, length);

        if (mStrings.contains(str)) {
            return str;
        }

        str.mData = allocate(data, length);
        mStrings.insert(str).first;
        return str;
    }

    String StringPool::intern(std::string_view string) {
        return intern(string.data(), string.length());
    }

    char* StringPool::allocate(const char* data, size_t length) {

    }
}
