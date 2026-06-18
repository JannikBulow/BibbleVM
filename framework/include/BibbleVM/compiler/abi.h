// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_COMPILER_ABI_H
#define BIBBLEVM_COMPILER_ABI_H 1

#include "BibbleVM/core/executor/function.h"

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
    };

    struct LeaveRegisters {
        uintptr_t exit0;
        uintptr_t exit1;
        uintptr_t exit2;
    };

    static_assert(sizeof(Frame) == 32);
    static_assert(offsetof(Frame, task) == 24);

    static_assert(sizeof(LeaveRegisters) == 24);
    static_assert(offsetof(LeaveRegisters, exit2) == 16);

#if defined(BIBBLEVM_ABI_X64_WIN64) || defined(BIBBLEVM_ABI_X64_SYSV)
    constexpr int FRAME_REGISTER = 13;
    constexpr int REGS_REGISTER = 12;
    constexpr int EXIT0_REGISTER = 0;
    constexpr int EXIT1_REGISTER = 1;
    constexpr int EXIT2_REGISTER = 2;
#elif defined(BIBBLEVM_ABI_AARCH64)
    constexpr int FRAME_REGISTER = 19;
    constexpr int REGS_REGISTER = 20;
    constexpr int EXIT0_REGISTER = 0;
    constexpr int EXIT1_REGISTER = 1;
    constexpr int EXIT2_REGISTER = 2;
#elif defined(BIBBLEVM_ABI_RISCV64)
    constexpr int FRAME_REGISTER = 8;
    constexpr int REGS_REGISTER = 9;
    constexpr int EXIT0_REGISTER = 10
    constexpr int EXIT1_REGISTER = 11
    constexpr int EXIT2_REGISTER = 12
#endif

    void bibblevm_enter(const Frame* frame, LeaveRegisters* leave);
}
}

#endif // BIBBLEVM_COMPILER_ABI_H
