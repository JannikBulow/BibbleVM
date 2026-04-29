// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/core/error.h"

#include "BibbleVM/vm.h"

namespace bibblevm {
    using namespace std::string_view_literals;

    std::string_view Error::TypeToString(Type type) {
        switch (type) {
            case USERLAND: return "USERLAND"sv;
            case NULL_REFERENCE: return "NULL_REFERENCE"sv;
            case INVALID_STATE: return "INVALID_STATE"sv;
            case INDEX_OUT_OF_BOUNDS: return "INDEX_OUT_OF_BOUNDS"sv;
            case INVALID_OBJECT_KIND: return "INVALID_OBJECT_KIND"sv;
        }

        __builtin_unreachable(); // TODO: macro this
    }

    String Error::describe(VM& vm) const {
        std::string_view typeString = TypeToString(type);
        size_t length = typeString.length() + 2 + (message == nullptr ? 4 : message.getLengthBytes());
        oop::StringObject* description = vm.memoryManager().allocateString(vm, length)->asString();
        if (description == nullptr) return nullptr;

        // Sorry. Ugly.
        size_t i = 0;
        memcpy(description->bytes, typeString.data(), typeString.length());
        i += typeString.length();
        description->bytes[i++] = '(';
        if (message != nullptr) {
            memcpy(description->bytes + i, message.getData(), message.getLengthBytes());
            i += message.getLengthBytes();
        } else {
            memcpy(description->bytes + i, "null", 4);
            i += 4;
        }
        description->bytes[i++] = ')';

        return description;
    }
}
