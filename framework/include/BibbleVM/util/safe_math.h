// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_UTIL_SAFE_MATH_H
#define BIBBLEVM_UTIL_SAFE_MATH_H 1

#include <cstdint>

namespace bibblevm::math {
    constexpr uint64_t LogicalShiftLeft(uint64_t value, uint32_t shift) {
        return value << (shift & 63);
    }

    constexpr uint64_t LogicalShiftRight(uint64_t value, uint32_t shift) {
        return value >> (shift & 63);
    }

    constexpr int64_t ArithmeticShiftRight(int64_t value, uint32_t shift) {
        shift &= 63;
        uint64_t uValue = static_cast<uint64_t>(value);
        uint64_t sign = uValue >> 63;
        uint64_t shifted = uValue >> shift;
        uint64_t extend = (sign << 63) >> (shift - 1);
        return static_cast<int64_t>(shifted | extend);
    }
}

#endif // BIBBLEVM_UTIL_SAFE_MATH_H
