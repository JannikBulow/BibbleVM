// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_CORE_OOP_STRING_H
#define BIBBLEVM_CORE_OOP_STRING_H 1

#include "BibbleVM/core/oop/object.h"

#include <cstring>
#include <format>
#include <iostream>
#include <string_view>

namespace bibblevm {
    class VM;
}

namespace bibblevm::oop {
    class BIBBLEVM_EXPORT String {
    public:
        struct Hash {
            using is_transparent = void;

            size_t operator()(const String& str) const noexcept {
                return std::hash<std::string_view>()(str);
            }

            size_t operator()(std::string_view str) const noexcept {
                return std::hash<std::string_view>()(str);
            }
        };

        struct Eq {
            using is_transparent = void;

            bool operator()(const String& str1, const String& str2) const noexcept {
                return str1 == str2;
            }

            bool operator()(const String& str1, std::string_view str2) const noexcept {
                return str1 == str2;
            }

            bool operator()(std::string_view str1, const String& str2) const noexcept {
                return str1 == str2;
            }
        };

        String(StringObject* object = nullptr) : mObject(object) {}

        String intern(VM& vm) const;

        StringObject* get() const { return mObject; }

        ULong getLengthBytes() const { return mObject->lengthBytes; }
        const char* getData() const { return mObject->bytes; }

        const char* cString() const { return mObject->bytes; } // memory manager automatically allocates with null terminator

        int compareFast(const String& other) const;
        int compareCorrect(const String& other) const;

        operator std::string_view() const {
            if (mObject == nullptr) return {};
            return {mObject->bytes, mObject->lengthBytes};
        }

        bool operator==(const String& other) const {
            if (mObject == other.mObject) return true;
            if (mObject == nullptr || other.mObject == nullptr) return false;
            return compareCorrect(other) == 0;
        }

        bool operator==(std::nullptr_t) const {
            return mObject == nullptr;
        }

    private:
        StringObject* mObject;
    };

    inline std::ostream& operator<<(std::ostream& os, const String& str) {
        return os << static_cast<std::string_view>(str);
    }
}

template<>
struct std::formatter<bibblevm::oop::String> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const bibblevm::oop::String& str, FormatContext& ctx) {
        return std::format_to(ctx.out(), "{}", static_cast<std::string_view>(str));
    }
};

#endif // BIBBLEVM_CORE_OOP_STRING_H
