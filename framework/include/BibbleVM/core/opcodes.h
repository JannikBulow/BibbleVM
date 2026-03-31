// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_OPCODES_H
#define BIBBLEVM_OPCODES_H 1

#include "BibbleVM/api.h"

#include <cstdint>

namespace bibblevm {
    enum Opcodes : uint8_t {
        // No operation.
        NOP = 0x0,

        // Move HOT register to HOT register.
        // a = dst
        // b = src
        MOV = 0x1,

        // Move HOT register to EXTENDED register.
        // a = dst_low
        // b = dst_high
        // c = src
        MOV_HOT_EXT = 0x02,

        // Move EXTENDED register to HOT register.
        // a = dst
        // b = src_low
        // c = src_high
        MOV_EXT_HOT = 0x03,

        // Move a range of registers to a range of registers. The range starts at the given registers, then continues to the right for `count` registers. Overlap is allowed and its behavior will be described in detail later.
        // a = dst
        // b = src
        // c = count
        MOV_RANGE = 0x04,

        // Swaps the value of two HOT registers.
        // a = rA
        // b = rB
        SWAP = 0x05,

        // Swaps the value of a HOT register with an EXTENDED register.
        // a = rA
        // b = rB_low
        // c = rB_high
        SWAP_HOT_EXT = 0x06,

        // Loads a constant value from the const pool of the currently executing function.
        // a = dst
        // b = cidx_low
        // c = cidx_high
        LOAD_CONST = 0x07,

        // Loads a single-byte immediate integer into an EXTENDED register.
        // a = dst_low
        // b = dst_high
        // c = imm
        LOADB = 0x0A,

        // Loads a two-byte immediate integer into a HOT register.
        // a = dst
        // b = imm_low
        // c = imm_high
        LOADS = 0x0B,

        // Add two 64-bit signed values together.
        // a = dst
        // b = lhs
        // c = rhs
        ADD = 0x10,

        // Subtract two 64-bit signed values.
        // a = dst
        // b = lhs
        // c = rhs
        SUB = 0x11,

        // Multiply two 64-bit signed values.
        // a = dst
        // b = lhs
        // c = rhs
        MUL = 0x12,

        // Divide two 64-bit signed values.
        // a = dst
        // b = lhs
        // c = rhs
        DIV = 0x13,

        // Modulo two 64-bit signed values.
        // a = dst
        // b = lhs
        // c = rhs
        MOD = 0x14,

        // Negate a 64-bit signed value.
        // a = dst
        // b = value
        NEG = 0x15,

        // Call a function located in a specified HOT register with a register range starting at a HOT or EXTENDED register.
        // The returned value of the callee is moved to R0.
        // a = func_reg
        // b = args_low
        // c = args_high
        CALL = 0xC0,

        // Same as CALL, except the current frame is reused (and possibly resized) for the callee.
        // The returned value of the callee is moved to R0.
        // a = func_reg
        // b = args_low
        // c = args_high
        TAIL_CALL = 0xC1,

        // Return from the current function, providing a register containing the returned value.
        // a = val_low
        // b = val_high
        RETURN = 0xC2,

        // Calls a function asynchronously and returns an uncompleted future in R0 that will eventually resolve with the returned value of the called function.
        // a = func_reg
        // b = args_low
        // c = args_high
        ASYNC_CALL = 0xC3,

        // Suspends the execution of the current task until the specified future has resolved.
        // Once the future has resolved, its value is moved to the destination register.
        // a = future_reg
        // b = dst_low
        // c = dst_high
        AWAIT = 0xC4,

        // Yields the current task, letting the next one in queue execute.
        YIELD = 0xC5,
    };
}

#endif //BIBBLEVM_OPCODES_H