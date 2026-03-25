// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_STRING_POOL_H
#define BIBBLEVM_STRING_POOL_H 1

#include "BibbleVM/allocator/arena.h"

#include "BibbleVM/api.h"

#include <format>
#include <iostream>
#include <string>
#include <unordered_set>

namespace bibblevm {
    class StringPool;

    // No substrings. A substring must be a separate string allocation
    class BIBBLEVM_EXPORT String {
    friend class StringPool;
    public:
        std::string_view asUsable() const {
            return {mData, mLength};
        }

        operator std::string_view() const {
            return asUsable();
        }

        bool operator==(const String& other) const {
            return mData == other.mData;
        }

        bool operator==(std::string_view other) const {
            return other == asUsable();
        }

    private:
        const char* mData;
        size_t mLength;

        String(const char* data, size_t length)
            : mData(data), mLength(length) {}

        struct Hash {
            size_t operator()(const String& str) const noexcept {
                return std::hash<std::string_view>()(str.asUsable());
            }
        };

        struct Eq {
            bool operator()(const String& str1, const String& str2) const noexcept {
                return str1.asUsable() == str2.asUsable();
            }
        };
    };

    inline std::ostream& operator<<(std::ostream& os, const String& str) {
        return os << str.asUsable();
    }

    class BIBBLEVM_EXPORT StringPool {
    public:
        StringPool();

        // If length == 0 this function calls strlen
        String intern(const char* data, size_t length = 0);
        String intern(std::string_view string);

    private:
        GrowingArenaAllocator mAllocator;
        std::unordered_set<String, String::Hash, String::Eq> mStrings;

        char* allocate(const char* data, size_t length);
    };
}

template<>
struct BIBBLEVM_EXPORT std::formatter<bibblevm::String> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const bibblevm::String& str, FormatContext& ctx) {
        return std::format_to(ctx.out(), "{}", str.asUsable());
    }
};

#endif // BIBBLEVM_STRING_POOL_H
