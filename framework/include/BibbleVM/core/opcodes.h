// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_OPCODES_H
#define BIBBLEVM_OPCODES_H 1

#include "BibbleVM/api.h"

#include <cstdint>

namespace bibblevm {
    using Opcode = uint8_t;

    enum Opcodes : Opcode {
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

        // Add two 64-bit values.
        // a = dst
        // b = lhs
        // c = rhs
        ADD = 0x10,

        // Subtract two 64-bit values.
        // a = dst
        // b = lhs
        // c = rhs
        SUB = 0x11,

        // Multiply two 64-bit values.
        // a = dst
        // b = lhs
        // c = rhs
        MUL = 0x12,

        // Divide two 64-bit signed values.
        // a = dst
        // b = lhs
        // c = rhs
        SDIV = 0x13,

        // Divide two 64-bit unsigned values.
        // a = dst
        // b = lhs
        // c = rhs
        UDIV = 0x14,

        // Modulo two 64-bit signed values.
        // a = dst
        // b = lhs
        // c = rhs
        SMOD = 0x15,

        // Modulo two 64-bit unsigned values.
        // a = dst
        // b = lhs
        // c = rhs
        UMOD = 0x16,

        // Negate a 64-bit signed value.
        // a = dst
        // b = value
        NEG = 0x17,

        // Get the absolute value of a 64-bit signed value.
        // a = dst
        // b = value
        ABS = 0x18,

        // Perform bitwise AND on two 64-bit values.
        // a = dst
        // b = lhs
        // c = rhs
        AND = 0x19,

        // Perform bitwise OR on two 64-bit values.
        // a = dst
        // b = lhs
        // c = rhs
        OR = 0x1A,

        // Perform bitwise XOR on two 64-bit values.
        // a = dst
        // b = lhs
        // c = rhs
        XOR = 0x1B,

        // Perform bitwise NOT on a 64-bit values.
        // a = dst
        // b = value
        NOT = 0x1C,

        // Shift a 64-bit value to the left.
        // a = dst
        // b = value
        // c = shift
        SHL = 0x1D,

        // Shift a 64-bit value to the right.
        // a = dst
        // b = value
        // c = shift
        SHR = 0x1E,

        // Shift a 64-bit value to the right while preserving the sign bit.
        // a = dst
        // b = value
        // c = shift
        SAR = 0x1F,

        // Add two 32-bit float values.
        // a = dst
        // b = lhs
        // c = rhs
        FADD = 0x20,

        // Subtract two 32-bit float values.
        // a = dst
        // b = lhs
        // c = rhs
        FSUB = 0x21,

        // Multiply two 32-bit float values.
        // a = dst
        // b = lhs
        // c = rhs
        FMUL = 0x22,

        // Divide two 32-bit float values.
        // a = dst
        // b = lhs
        // c = rhs
        FDIV = 0x23,

        // Negate a 32-bit float value.
        // a = dst
        // b = value
        FNEG = 0x24,

        // Get the absolute value of a 32-bit float value.
        // a = dst
        // b = value
        FABS = 0x25,

        // Add two 64-bit float values.
        // a = dst
        // b = lhs
        // c = rhs
        DADD = 0x26,

        // Subtract two 64-bit float values.
        // a = dst
        // b = lhs
        // c = rhs
        DSUB = 0x27,

        // Multiply two 64-bit float values.
        // a = dst
        // b = lhs
        // c = rhs
        DMUL = 0x28,

        // Divide two 64-bit float values.
        // a = dst
        // b = lhs
        // c = rhs
        DDIV = 0x29,

        // Negate a 64-bit float value.
        // a = dst
        // b = value
        DNEG = 0x2A,

        // Get the absolute value of a 64-bit float value.
        // a = dst
        // b = value
        DABS = 0x2B,

        // Truncate a 64-bit integer down to 8 bits.
        // a = dst
        // b = value
        TR8 = 0x2C,

        // Truncate a 64-bit integer down to 8 bits while preserving signedness.
        // a = dst
        // b = value
        TR8S = 0x2D,

        // Truncate a 64-bit integer down to 16 bits.
        // a = dst
        // b = value
        TR16 = 0x2E,

        // Truncate a 64-bit integer down to 16 bits while preserving signedness.
        // a = dst
        // b = value
        TR16S = 0x2F,

        // Truncate a 64-bit integer down to 32 bits.
        // a = dst
        // b = value
        TR32 = 0x30,

        // Truncate a 64-bit integer down to 32 bits while preserving signedness.
        // a = dst
        // b = value
        TR32S = 0x31,

        // Sign-extend an 8-bit integer to a 64-bit integer.
        // If you read 'SEX' and think of anything other than Sign EXtend, you need to lock in.
        // a = dst
        // b = value
        SEX8 = 0x32,

        // Sign-extend a 16-bit integer to a 64-bit integer.
        // If you read 'SEX' and think of anything other than Sign EXtend, you need to lock in.
        // a = dst
        // b = value
        SEX16 = 0x32,

        // Sign-extend a 32-bit integer to a 64-bit integer.
        // If you read 'SEX' and think of anything other than Sign EXtend, you need to lock in.
        // a = dst
        // b = value
        SEX32 = 0x32,

        // Zero-extend an 8-bit integer to a 64-bit integer.
        // a = dst
        // b = value
        ZEX8 = 0x32,

        // Zero-extend a 16-bit integer to a 64-bit integer.
        // a = dst
        // b = value
        ZEX16 = 0x32,

        // Zero-extend a 32-bit integer to a 64-bit integer.
        // a = dst
        // b = value
        ZEX32 = 0x32,

        // Convert a 64-bit integer to a 32-bit float.
        // a = dst
        // b = value
        I2F = 0x33,

        // Convert a 64-bit integer to a 64-bit float.
        // a = dst
        // b = value
        I2D = 0x34,

        // Convert a 32-bit float to a 64-bit integer.
        // a = dst
        // b = value
        F2I = 0x35,

        // Convert a 64-bit float to a 64-bit integer.
        // a = dst
        // b = value
        D2I = 0x36,

        // Promote a 32-bit float to a 64-bit float.
        // a = dst
        // b = value
        F2D = 0x37,

        // Demote a 64-bit float to a 32-bit float.
        // a = dst
        // b = value
        D2F = 0x38,

        // Compare 2 64-bit signed integers and produce a special comparison value used by conditional jump instructions.
        // a = dst
        // b = lhs
        // c = rhs
        ICMP = 0x39,

        // Compare 2 64-bit unsigned integers and produce a special comparison value used by conditional jump instructions.
        // a = dst
        // b = lhs
        // c = rhs
        UCMP = 0x40,

        // Compare 2 32-bit floats and produce a special comparison value used by conditional jump instructions.
        // a = dst
        // b = lhs
        // c = rhs
        FCMP = 0x41,

        // Compare 2 64-bit floats and produce a special comparison value used by conditional jump instructions.
        // a = dst
        // b = lhs
        // c = rhs
        DCMP = 0x42,

        // Compare 2 strings and produce a special comparison value used by conditional jump instructions.
        // The produced value of STRCMP is the same as the strcmp() function in C, just operating on UTF-8 codepoints instead of bytes.
        // a = dst
        // b = lhs
        // c = rhs
        STRCMP = 0x43,

        // Increment a given 64-bit integer by a 16-bit immediate unsigned integer.
        // a = reg
        // b = value_low
        // c = value_high
        INC = 0x44,

        // Decrement a given 64-bit integer by a 16-bit immediate unsigned integer.
        // a = reg
        // b = value_low
        // c = value_high
        DEC = 0x45,

        // Advance the instruction pointer.
        // a = branch_low
        // b = branch_mid
        // c = branch_high
        JMP = 0x50,

        // Advance the instruction pointer if the 8-bit signed integer specified in the value register is equal to zero.
        // The condition might be weirdly worded, so just know that the CMP instructions produce 0 if both values are equal.
        // a = value
        // b = branch_low
        // c = branch_high
        JEQ = 0x51,

        // Advance the instruction pointer if the 8-bit signed integer specified in the value register is not equal to zero.
        // The condition might be weirdly worded, so just know that the CMP instructions produce 0 if both values are equal.
        // a = value
        // b = branch_low
        // c = branch_high
        JNE = 0x52,

        // Advance the instruction pointer if the 8-bit signed integer specified in the value register is less than zero.
        // The condition might be weirdly worded, so just know that the CMP instructions produce -1 if lhs is less than rhs.
        // a = value
        // b = branch_low
        // c = branch_high
        JLT = 0x53,

        // Advance the instruction pointer if the 8-bit signed integer specified in the value register is less than or equal to zero.
        // The condition might be weirdly worded, so just know that the CMP instructions produce -1 if lhs is less than rhs and 0 if they are equal.
        // a = value
        // b = branch_low
        // c = branch_high
        JLE = 0x54,

        // Advance the instruction pointer if the 8-bit signed integer specified in the value register is greater than zero.
        // The condition might be weirdly worded, so just know that the CMP instructions produce 1 if lhs is greater than rhs.
        // a = value
        // b = branch_low
        // c = branch_high
        JGT = 0x55,

        // Advance the instruction pointer if the 8-bit signed integer specified in the value register is greater than or equal to zero.
        // The condition might be weirdly worded, so just know that the CMP instructions produce 1 if lhs is greater than rhs and 0 if they are equal.
        // a = value
        // b = branch_low
        // c = branch_high
        JGE = 0x56,

        // 4 opcodes reserved for future switch instructions with different strategies.
        RESERVED_FOR_SWITCH_0 = 0x57,
        RESERVED_FOR_SWITCH_1 = 0x58,
        RESERVED_FOR_SWITCH_2 = 0x59,
        RESERVED_FOR_SWITCH_3 = 0x5A,

        // Dynamically allocate a new uninitialized instance object based on the given class info constant.
        // a = dst
        // b = cidx_low
        // c = cidx_high
        NEWINSTANCE = 0x60,

        // Dynamically allocate a new zero-initialized array object with elements based on the given type id
        // and a size specified in the length_reg register.
        // a = dst
        // b = typeid
        // c = length_reg
        NEWARRAY = 0x61,

        // Dynamically allocate a new zero-initialized string object from a given byte array
        // consisting of UTF-8 codepoints in the data_reg register.
        // The string is constructed with the same length as the data array, then the data from the array is copied into the string.
        // a = dst
        // b = data_reg
        NEWSTRING = 0x62,

        // Dynamically allocate a new uncompleted future object.
        // This is not really useful, as async/await opcodes will make these automatically.
        // a = dst
        NEWFUTURE = 0x63,

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

        // Calls a function asynchronously with the priority level of the caller and returns an uncompleted future in R0
        // that will eventually resolve with the returned value of the called function.
        // Name means "CALL ASYNC."
        // a = func_reg
        // b = args_low
        // c = args_high
        CALLA = 0xC3,

        // Calls a function asynchronously with a priority level specified in the priority_reg register and returns
        // an uncompleted future in R0 that will eventually resolve with the returned value of the called function.
        // The priority level will be truncated down to an unsigned 8-bit integer in a similar way to TR8.
        // Name means "CALL ASYNC PRIORITY."
        // a = func_reg
        // b = priority_reg
        // c = args
        CALLAP = 0xC4,

        // Calls a function asynchronously with a priority level of the caller plus the specified 8-bit signed immediate
        // in the priority argument and returns an uncompleted future in R0 that will eventually resolve with
        // the returned value of the called function.
        // Name means "CALL ASYNC RELATIVE PRIORITY."
        // a = func_reg
        // b = priority
        // c = args
        CALLARP = 0xC5,

        // Suspends the execution of the current task until the specified future has resolved.
        // Once the future has resolved, its value is moved to the destination register.
        // a = future_reg
        // b = dst_low
        // c = dst_high
        AWAIT = 0xC6,

        // Yields the current task, letting the next one in queue execute.
        YIELD = 0xC7,
    };

    namespace opcodeutils {
        // Including opcode. 0 means variable length, which needs a bit more context to determine size.
        size_t GetFixedLength(Opcode opcode);
    }
}

#endif //BIBBLEVM_OPCODES_H