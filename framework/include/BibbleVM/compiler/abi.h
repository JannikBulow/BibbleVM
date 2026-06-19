// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_COMPILER_ABI_H
#define BIBBLEVM_COMPILER_ABI_H 1

#include "BibbleVM/core/executor/function.h"

#include <array>

#if defined(_M_X64) || defined(__x86_64__)
#define BIBBLEVM_ARCH_X64
#elif defined(__aarch64__)
#define BIBBLEVM_ARCH_AARCH64
#elif defined(__riscv) && (__riscv_xlen == 64)
#define BIBBLEVM_ARCH_RISCV64
#else
#error Unsupported architecture
#endif

#if defined(PLATFORM_WINDOWS) && defined(BIBBLEVM_ARCH_X64)
#define BIBBLEVM_ABI_X64_WIN64
#elif defined(PLATFORM_UNIX) && defined(BIBBLEVM_ARCH_X64)
#define BIBBLEVM_ABI_X64_SYSV
#elif defined(BIBBLEVM_ARCH_AARCH64)
#define BIBBLEVM_ABI_AARCH64
#elif defined(BIBBLEVM_ARCH_RISCV64)
#define BIBBLEVM_ABI_RISCV64
#else
#error Unsupported ABI
#endif

extern "C" {
namespace bibblevm::compiler::abi {
    struct Frame {
        VM* vm;
        Value* registers;

        executor::Frame* frame;
        executor::Task* task;

        void* code;
    };

    struct LeaveRegisters {
        uintptr_t reason;
        uintptr_t exit1;
        uintptr_t exit2;
    };

    enum class LeaveReason {
        Error = 0,
        Return = 1,
    };

    static_assert(sizeof(Frame) == 40);
    static_assert(offsetof(Frame, code) == 32);

    static_assert(sizeof(LeaveRegisters) == 24);
    static_assert(offsetof(LeaveRegisters, exit2) == 16);

#if defined(BIBBLEVM_ABI_X64_WIN64) || defined(BIBBLEVM_ABI_X64_SYSV)
    constexpr int FRAME_REGISTER = 14;
    constexpr int REGS_REGISTER = 15;
    constexpr int EXIT0_REGISTER = 0;
    constexpr int EXIT1_REGISTER = 1;
    constexpr int EXIT2_REGISTER = 2;

    // allowed to use
    constexpr std::array GPRS = {
        0, 1, 2, 3, 5, 6, 7, 8, 9, 10, 11, 12, 13
    };

    // never touch
    constexpr std::array RESERVED_REGISTERS = {
        4, 14, 15
    };

#if defined(BIBBLEVM_ABI_X64_WIN64)
    constexpr std::array PLATFORM_ABI_VOLATILE_REGISTERS = {
        0, 1, 2, 8, 9, 10, 11
    };
#else
    constexpr std::array PLATFORM_ABI_VOLATILE_REGISTERS = {
        0, 1, 2, 6, 7, 8, 9, 10, 11
    };
#endif
#elif defined(BIBBLEVM_ABI_AARCH64)
    //TODO: aarch64 abi
#elif defined(BIBBLEVM_ABI_RISCV64)
//TODO: riscv64 abi
#endif

    void bibblevm_enter(const Frame* frame, LeaveRegisters* leave);
}
}

#endif // BIBBLEVM_COMPILER_ABI_H
