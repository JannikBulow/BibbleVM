// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM__COMPATABILITY_H
#define BIBBLEVM__COMPATABILITY_H 1

namespace bibblevm::compat {
#if defined(__GNUC__) || defined(__clang__)
#   define BIBBLEVM_UNREACHABLE() __builtin_unreachable()
#elif defined(_MSC_VER)
#   define BIBBLEVM_UNREACHABLE() __assume(false)
#else
    #   define BIBBLEVM_UNREACHABLE() std::exit(34)
#endif
}

#endif // BIBBLEVM__COMPATABILITY_H
