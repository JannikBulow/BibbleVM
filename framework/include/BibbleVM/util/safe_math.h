// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_UTIL_SAFE_MATH_H
#define BIBBLEVM_UTIL_SAFE_MATH_H 1

#include <cstdint>

// UB-less math in the language made by John UB.
namespace bibblevm::math {
    template<class T>
    struct FPTraits;

    template<>
    struct FPTraits<float> {
        using UInt = uint32_t;
        static constexpr int EXP_BITS = 8;
        static constexpr int FRAC_BITS = 23;
        static constexpr int EXP_BIAS = 127;
    };

    template<>
    struct FPTraits<double> {
        using UInt = uint64_t;
        static constexpr int EXP_BITS = 11;
        static constexpr int FRAC_BITS = 52;
        static constexpr int EXP_BIAS = 1023;
    };

    template<class T>
    struct IntTraits;

    template<>
    struct IntTraits<int64_t> {
        static constexpr bool SIGNED = true;
        static constexpr int64_t MIN = INT64_MIN;
        static constexpr int64_t MAX = INT64_MAX;
    };

    template<>
    struct IntTraits<uint64_t> {
        static constexpr bool SIGNED = false;
        static constexpr uint64_t MIN = 0;
        static constexpr uint64_t MAX = UINT64_MAX;
    };

    template<class FP, class INT>
    INT FPToInt(FP value) {
        using FPTraits = FPTraits<FP>;
        using INTTraits = IntTraits<INT>;
        using UInt = FPTraits::UInt;

        constexpr UInt MAX_EXP = (1u << FPTraits::EXP_BITS) - 1;

        UInt bits = std::bit_cast<UInt>(value);

        UInt sign = bits >> (FPTraits::EXP_BITS + FPTraits::FRAC_BITS);
        UInt exp = (bits >> FPTraits::FRAC_BITS) & ((1u << FPTraits::EXP_BITS) - 1);
        UInt frac = bits & ((UInt(1) << FPTraits::FRAC_BITS) - 1);

        if (exp == MAX_EXP) {
            if (frac != 0) return 0;

            return sign ? INTTraits::MIN : INTTraits::MAX;
        }

        if (exp == 0) return 0;

        uint64_t mant = (static_cast<uint64_t>(1) << FPTraits::FRAC_BITS) | frac;
        int32_t e = static_cast<int32_t>(exp) - FPTraits::EXP_BIAS;
        int32_t shift = e - FPTraits::FRAC_BITS;
        uint64_t absValue;

        if (shift >= 0) {
            if (shift > 63) {
                return sign ? INTTraits::MIN : INTTraits::MAX;
            }
            absValue = mant << shift;
        } else {
            if (shift < -63) return 0;
            absValue = mant >> (-shift);
        }

        if constexpr (INTTraits::SIGNED) {
            if (sign) {
                if (absValue > static_cast<uint64_t>(INTTraits::MAX) + 1) {
                    return INTTraits::MIN;
                }
                return -INT(absValue);
            } else {
                if (absValue > static_cast<uint64_t>(INTTraits::MAX)) {
                    return INTTraits::MAX;
                }
                return INT(absValue);
            }
        } else {
            if (sign) return 0;
            if (absValue > INTTraits::MAX) return INTTraits::MAX;
            return INT(absValue);
        }
    }

    template<class FP, class INT>
    FP IntToFP(INT value) {
        return static_cast<FP>(value); // shouldn't have any ub but maybe i'll implement this from scratch like above
    }

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
