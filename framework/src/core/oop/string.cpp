// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/core/oop/string.h"

#include "BibbleVM/vm.h"

#include <utf8.h>

namespace bibblevm::oop {
    String String::intern(VM& vm) const {
        return vm.stringPool().intern(vm, *this);
    }

    int String::compareFast(const String& other) const {
        ULong minLength = std::min(mObject->lengthBytes, other.mObject->lengthBytes);

        int r = memcmp(mObject->bytes, other.mObject->bytes, minLength);
        if (r != 0)
            return r;

        if (mObject->lengthBytes == other.mObject->lengthBytes)
            return 0;

        return (mObject->lengthBytes < other.mObject->lengthBytes) ? -1 : 1;
    }

    int String::compareCorrect(const String& other) const {
        auto itA = mObject->bytes;
        auto itB = other.mObject->bytes;
        auto endA = mObject->bytes + mObject->lengthBytes;
        auto endB = other.mObject->bytes + other.mObject->lengthBytes;

        while (itA < endA && itB < endB) {
            utf8::utfchar32_t cpA = utf8::next(itA, endA);
            utf8::utfchar32_t cpB = utf8::next(itB, endB);

            if (cpA != cpB)
                return (cpA < cpB) ? -1 : 1;
        }

        if (itA == endA && itB == endB) return 0;
        return (itA < endA) ? 1 : -1;
    }
}
