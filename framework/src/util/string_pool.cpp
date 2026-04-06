// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/util/string_pool.h"

#include "BibbleVM/vm.h"

#include <cstring>

namespace bibblevm {
    String StringPool::intern(VM& vm, const char* data, size_t length) {
        if (length == 0) length = strlen(data);
        return intern(vm, {data, length});
    }

    String StringPool::intern(VM& vm, std::string_view string) {
        auto it = mStrings.find(string);
        if (it != mStrings.end()) return *it;

        oop::Object* stringObject = vm.memoryManager().allocateImmortalString(string);
        String str = stringObject->asString();

        mStrings.insert(str);

        return str;
    }
}
