// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_OPCODES_H
#define BIBBLEVM_OPCODES_H 1

#include "BibbleVM/api.h"

#include <cstdint>

namespace bibblevm {
    using Prefix = uint8_t;
    using Opcode = uint8_t;

    constexpr uint8_t PREFIX_MASK = 0xF0;

    enum Prefixes : Prefix {
        WIDE_OPERAND0 = 0xF0,
        WIDE_OPERAND1 = 0xF1,
        WIDE_OPERAND2 = 0xF2,
        WIDE_OPERAND3 = 0xF3,
        HUGE_IMMEDIATE = 0xF4,
        GIGANTIC_IMMEDIATE = 0xF5,
    };

    enum Opcodes : Opcode {
        /*
        INSTRUCTION: NOP

        PURPOSE:
            Perform no operation and waste a cycle.

        OPERANDS:
            none

        ENCODING:
            PREFIXES:
                none

            LAYOUT:
                [PREFIX*] [NOP]
        */
        NOP = 0x0,

        /*
        INSTRUCTION: MOV

        PURPOSE:
            Copy a register into another register.

        SEMANTICS:
            dst = src

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16-bit register address
                    DEFAULT: 8-bit register address

            SRC:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0: dst width control
                WIDE_OPERAND0: src width control

            LAYOUT:
                [PREFIX*] [MOV] [DST] [SRC]
        */
        MOV = 0x1,

        /*
        INSTRUCTION: MOV_RANGE

        PURPOSE:
            Move a range of registers to another range of registers.

        SEMANTICS:
            for i = 0; i < COUNT; i += 1:
                REG[DST + i] = REG[SRC + i]

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            SRC:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

            COUNT:
                TYPE: immediate
                SIZE:
                    WIDE_OPERAND2: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1
                WIDE_OPERAND2

            LAYOUT:
                [PREFIX*] [MOV_RANGE] [DST] [SRC] [COUNT]
        */
        MOV_RANGE = 0x02,

        /*
        INSTRUCTION: SWAP

        PURPOSE:
            Swap the values of two registers.

        SEMANTICS:
            let tmp = REG[A]
            REG[A] = REG[B]
            REG[B] = tmp

        OPERANDS:
            A:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            B:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1

            LAYOUT:
                [PREFIX*] [SWAP] [A] [B]
        */
        SWAP = 0x03,

        /*
        INSTRUCTION: LOAD_CONST

        PURPOSE:
            Load a value from the constant pool of the currently executing function.

        SEMANTICS:
            REG[DST] = CONST[IDX]

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            IDX:
                TYPE: const-pool index
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1

            LAYOUT:
                [PREFIX*] [LOAD_CONST] [DST] [IDX]
        */
        LOAD_CONST = 0x04,

        /*
        INSTRUCTION: LOAD_IMM

        PURPOSE:
            Load an immediate integer value into a register.

        SEMANTICS:
            REG[DST] = IMM

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            IMM:
                TYPE: immediate
                SIZE:
                    GIGANTIC_IMMEDIATE: 64 bits
                    HUGE_IMMEDIATE: 32 bits
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1
                HUGE_IMMEDIATE
                GIGANTIC_IMMEDIATE

            LAYOUT:
                [PREFIX*] [LOAD_IMM] [DST] [IMM]
        */
        LOAD_IMM = 0x05,

        /*
        INSTRUCTION: ADD

        PURPOSE:
            Add two values together.

        SEMANTICS:
            REG[DST] = REG[LHS] + REG[RHS]

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            LHS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

            RHS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND2: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1
                WIDE_OPERAND2

            LAYOUT:
                [PREFIX*] [ADD] [DST] [LHS] [RHS]
        */
        ADD = 0x10,

        /*
        INSTRUCTION: SUB

        PURPOSE:
            Subtract two values.

        SEMANTICS:
            REG[DST] = REG[LHS] - REG[RHS]

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            LHS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

            RHS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND2: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1
                WIDE_OPERAND2

            LAYOUT:
                [PREFIX*] [SUB] [DST] [LHS] [RHS]
        */
        SUB = 0x11,

        /*
        INSTRUCTION: MUL

        PURPOSE:
            Multiply two values together.

        SEMANTICS:
            REG[DST] = REG[LHS] * REG[RHS]

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            LHS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

            RHS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND2: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1
                WIDE_OPERAND2

            LAYOUT:
                [PREFIX*] [MUL] [DST] [LHS] [RHS]
        */
        MUL = 0x12,

        /*
        INSTRUCTION: SDIV

        PURPOSE:
            Divide two signed values.

        SEMANTICS:
            REG[DST] = (signed) REG[LHS] / (signed) REG[RHS]

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            LHS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

            RHS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND2: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1
                WIDE_OPERAND2

            LAYOUT:
                [PREFIX*] [SDIV] [DST] [LHS] [RHS]
        */
        SDIV = 0x13,

        /*
        INSTRUCTION: UDIV

        PURPOSE:
            Divide two unsigned values.

        SEMANTICS:
            REG[DST] = (unsigned) REG[LHS] / (unsigned) REG[RHS]

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            LHS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

            RHS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND2: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1
                WIDE_OPERAND2

            LAYOUT:
                [PREFIX*] [UDIV] [DST] [LHS] [RHS]
        */
        UDIV = 0x14,

        /*
        INSTRUCTION: SMOD

        PURPOSE:
            Modulo two signed values.

        SEMANTICS:
            REG[DST] = (signed) REG[LHS] % (signed) REG[RHS]

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            LHS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

            RHS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND2: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1
                WIDE_OPERAND2

            LAYOUT:
                [PREFIX*] [SMOD] [DST] [LHS] [RHS]
        */
        SMOD = 0x15,

        /*
        INSTRUCTION: UMOD

        PURPOSE:
            Modulo two unsigned values.

        SEMANTICS:
            REG[DST] = (unsigned) REG[LHS] % (unsigned) REG[RHS]

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            LHS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

            RHS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND2: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1
                WIDE_OPERAND2

            LAYOUT:
                [PREFIX*] [UMOD] [DST] [LHS] [RHS]
        */
        UMOD = 0x16,

        /*
        INSTRUCTION: NEG

        PURPOSE:
            Negate a value.

        SEMANTICS:
            REG[DST] = -REG[VALUE]

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            VALUE:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1

            LAYOUT:
                [PREFIX*] [NEG] [DST] [VALUE]
        */
        NEG = 0x17,

        /*
        INSTRUCTION: ABS

        PURPOSE:
            Get the absolute value of a value.

        SEMANTICS:
            if REG[VALUE] < 0:
                REG[DST] = -REG[VALUE]
            else:
                REG[DST] = REG[VALUE]

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            VALUE:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1

            LAYOUT:
                [PREFIX*] [ABS] [DST] [VALUE]
        */
        ABS = 0x18,

        /*
        INSTRUCTION: AND

        PURPOSE:
            Perform bitwise AND on two values.

        SEMANTICS:
            REG[DST] = REG[LHS] & REG[RHS]

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            LHS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

            RHS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND2: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1
                WIDE_OPERAND2

            LAYOUT:
                [PREFIX*] [AND] [DST] [LHS] [RHS]
        */
        AND = 0x19,

        /*
        INSTRUCTION: OR

        PURPOSE:
            Perform bitwise OR on two values.

        SEMANTICS:
            REG[DST] = REG[LHS] | REG[RHS]

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            LHS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

            RHS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND2: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1
                WIDE_OPERAND2

            LAYOUT:
                [PREFIX*] [OR] [DST] [LHS] [RHS]
        */
        OR = 0x1A,

        /*
        INSTRUCTION: XOR

        PURPOSE:
            Perform bitwise XOR on two values.

        SEMANTICS:
            REG[DST] = REG[LHS] ^ REG[RHS]

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            LHS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

            RHS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND2: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1
                WIDE_OPERAND2

            LAYOUT:
                [PREFIX*] [XOR] [DST] [LHS] [RHS]
        */
        XOR = 0x1B,

        /*
        INSTRUCTION: NOT

        PURPOSE:
            Perform bitwise NOT on a value.

        SEMANTICS:
            REG[DST] = ~REG[VALUE]

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            VALUE:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1

            LAYOUT:
                [PREFIX*] [NOT] [DST] [VALUE]
        */
        NOT = 0x1C,

        /*
        INSTRUCTION: SHL

        PURPOSE:
            Perform bitwise shifting to the left on a value using another value as the shift amount.

        SEMANTICS:
            REG[DST] = REG[LHS] << REG[RHS]

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            LHS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

            RHS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND2: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1
                WIDE_OPERAND2

            LAYOUT:
                [PREFIX*] [SHL] [DST] [LHS] [RHS]
        */
        SHL = 0x1D,

        /*
        INSTRUCTION: SHR

        PURPOSE:
            Perform bitwise shifting to the right on a value using another value as the shift amount.

        SEMANTICS:
            REG[DST] = REG[LHS] >> REG[RHS]

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            LHS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

            RHS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND2: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1
                WIDE_OPERAND2

            LAYOUT:
                [PREFIX*] [SHR] [DST] [LHS] [RHS]
        */
        SHR = 0x1E,

        /*
        INSTRUCTION: SHL

        PURPOSE:
            Perform arithmetic shifting to the right on a value using another value as the shift amount.

        SEMANTICS:
            REG[DST] = REG[LHS] << REG[RHS]

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            LHS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

            RHS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND2: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1
                WIDE_OPERAND2

            LAYOUT:
                [PREFIX*] [SHL] [DST] [LHS] [RHS]

        NOTES:
            - There is no "left" counterpart to this instruction as it would do the exact same.
            - The difference between bitwise and arithmetic shifting is that arithmetic shifting will preserve the sign bit on signed values.
        */
        SAR = 0x1F,

        /*
        INSTRUCTION: FADD

        PURPOSE:
            Add two float values together.

        SEMANTICS:
            REG[DST] = REG[LHS] + REG[RHS]

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            LHS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

            RHS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND2: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1
                WIDE_OPERAND2

            LAYOUT:
                [PREFIX*] [FADD] [DST] [LHS] [RHS]
        */
        FADD = 0x20,

        /*
        INSTRUCTION: FSUB

        PURPOSE:
            Subtract two float values.

        SEMANTICS:
            REG[DST] = REG[LHS] - REG[RHS]

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            LHS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

            RHS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND2: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1
                WIDE_OPERAND2

            LAYOUT:
                [PREFIX*] [FSUB] [DST] [LHS] [RHS]
        */
        FSUB = 0x21,

        /*
        INSTRUCTION: FMUL

        PURPOSE:
            Multiply two float values together.

        SEMANTICS:
            REG[DST] = REG[LHS] * REG[RHS]

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            LHS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

            RHS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND2: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1
                WIDE_OPERAND2

            LAYOUT:
                [PREFIX*] [FMUL] [DST] [LHS] [RHS]
        */
        FMUL = 0x22,

        /*
        INSTRUCTION: FDIV

        PURPOSE:
            Divide two float values.

        SEMANTICS:
            REG[DST] = REG[LHS] / REG[RHS]

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            LHS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

            RHS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND2: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1
                WIDE_OPERAND2

            LAYOUT:
                [PREFIX*] [FDIV] [DST] [LHS] [RHS]
        */
        FDIV = 0x23,

        /*
        INSTRUCTION: FNEG

        PURPOSE:
            Negate a float value.

        SEMANTICS:
            REG[DST] = -REG[VALUE]

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            VALUE:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1

            LAYOUT:
                [PREFIX*] [FNEG] [DST] [VALUE]
        */
        FNEG = 0x24,

        /*
        INSTRUCTION: FABS

        PURPOSE:
            Get the absolute value of a float value.

        SEMANTICS:
            if REG[VALUE] < 0:
                REG[DST] = -REG[VALUE]
            else:
                REG[DST] = REG[VALUE]

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            VALUE:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1

            LAYOUT:
                [PREFIX*] [FABS] [DST] [VALUE]
        */
        FABS = 0x25,

        /*
        INSTRUCTION: TR8

        PURPOSE:
            Truncate a 64-bit unsigned integer value down to 8 bits.

        SEMANTICS:
            REG[DST] = REG[VALUE] & 0xFF

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            VALUE:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1

            LAYOUT:
                [PREFIX*] [TR8] [DST] [VALUE]
        */
        TR8 = 0x2C,

        /*
        INSTRUCTION: TR8

        PURPOSE:
            Truncate a 64-bit unsigned integer value down to 16 bits.

        SEMANTICS:
            REG[DST] = REG[VALUE] & 0xFFFF

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            VALUE:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1

            LAYOUT:
                [PREFIX*] [TR16] [DST] [VALUE]
        */
        TR16 = 0x2E,

        /*
        INSTRUCTION: TR32

        PURPOSE:
            Truncate a 64-bit unsigned integer value down to 32 bits.

        SEMANTICS:
            REG[DST] = REG[VALUE] & 0xFFFFFFFF

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            VALUE:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1

            LAYOUT:
                [PREFIX*] [TR32] [DST] [VALUE]
        */
        TR32 = 0x30,

        /*
        INSTRUCTION: SEX8

        PURPOSE:
            Sign-extend an 8-bit integer to a 64-bit integer.

        SEMANTICS:
            let value = REG[VALUE] & 0xFF
            if value & 0x80 != 0:
                REG[DST] = value | 0xFFFFFFFFFFFFFF00
            else:
                REG[DST] = value

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            VALUE:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1

            LAYOUT:
                [PREFIX*] [SEX8] [DST] [VALUE]

        NOTES:
            - If you read 'SEX' and think of anything other than Sign EXtend, you need to lock in.
        */
        SEX8 = 0x32,

        /*
        INSTRUCTION: SEX16

        PURPOSE:
            Sign-extend an 16-bit integer to a 64-bit integer.

        SEMANTICS:
            let value = REG[VALUE] & 0xFFFF
            if value & 0x8000 != 0:
                REG[DST] = value | 0xFFFFFFFFFFFF0000
            else:
                REG[DST] = value

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            VALUE:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1

            LAYOUT:
                [PREFIX*] [SEX16] [DST] [VALUE]

        NOTES:
            - If you read 'SEX' and think of anything other than Sign EXtend, you need to lock in.
        */
        SEX16 = 0x33,

        /*
        INSTRUCTION: SEX32

        PURPOSE:
            Sign-extend an 32-bit integer to a 64-bit integer.

        SEMANTICS:
            let value = REG[VALUE] & 0xFFFFFFFF
            if value & 0x80000000 != 0:
                REG[DST] = value | 0xFFFFFFFF00000000
            else:
                REG[DST] = value

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            VALUE:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1

            LAYOUT:
                [PREFIX*] [SEX32] [DST] [VALUE]

        NOTES:
            - If you read 'SEX' and think of anything other than Sign EXtend, you need to lock in.
        */
        SEX32 = 0x34,

        /*
        INSTRUCTION: ZEX8

        PURPOSE:
            Zero-extend an 8-bit integer to a 64-bit integer.

        SEMANTICS:
            REG[DST] = REG[VALUE] & 0xFF

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            VALUE:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1

            LAYOUT:
                [PREFIX*] [ZEX8] [DST] [VALUE]
        */
        ZEX8 = 0x35,

        /*
        INSTRUCTION: ZEX16

        PURPOSE:
            Zero-extend an 16-bit integer to a 64-bit integer.

        SEMANTICS:
            REG[DST] = REG[VALUE] & 0xFFFF

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            VALUE:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1

            LAYOUT:
                [PREFIX*] [ZEX16] [DST] [VALUE]
        */
        ZEX16 = 0x36,

        /*
        INSTRUCTION: ZEX32

        PURPOSE:
            Zero-extend an 32-bit integer to a 64-bit integer.

        SEMANTICS:
            REG[DST] = REG[VALUE] & 0xFFFFFFFF

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            VALUE:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1

            LAYOUT:
                [PREFIX*] [ZEX32] [DST] [VALUE]
        */
        ZEX32 = 0x37,

        /*
        INSTRUCTION: I2F

        PURPOSE:
            Convert a 64-bit signed integer to a 32-bit float.

        SEMANTICS:
            REG[DST] = IEEE754_s64_to_binary32(REG[VALUE])

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            VALUE:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1

            LAYOUT:
                [PREFIX*] [I2F] [DST] [VALUE]

        NOTES:
            - Conversion may lose precision.
            - Values outside the exact representable range of IEEE 754 binary32 are rounded.
        */
        I2F = 0x38,

        /*
        INSTRUCTION: U2F

        PURPOSE:
            Convert a 64-bit unsigned integer to a 32-bit float.

        SEMANTICS:
            REG[DST] = IEEE754_u64_to_binary32(REG[VALUE])

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            VALUE:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1

            LAYOUT:
                [PREFIX*] [U2F] [DST] [VALUE]

        NOTES:
            - Conversion may lose precision.
            - Values outside the exact representable range of IEEE 754 binary32 are rounded.
        */
        U2F = 0x39,

        /*
        INSTRUCTION: I2D

        PURPOSE:
            Convert a 64-bit signed integer to a 64-bit float.

        SEMANTICS:
            REG[DST] = IEEE754_s64_to_binary64(REG[VALUE])

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            VALUE:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1

            LAYOUT:
                [PREFIX*] [I2D] [DST] [VALUE]

        NOTES:
            - Conversion may lose precision.
            - Values outside the exact representable range of IEEE 754 binary64 are rounded.
        */
        I2D = 0x3A,

        /*
        INSTRUCTION: U2D

        PURPOSE:
            Convert a 64-bit unsigned integer to a 64-bit float.

        SEMANTICS:
            REG[DST] = IEEE754_u64_to_binary64(REG[VALUE])

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            VALUE:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1

            LAYOUT:
                [PREFIX*] [U2D] [DST] [VALUE]

        NOTES:
            - Conversion may lose precision.
            - Values outside the exact representable range of IEEE 754 binary64 are rounded.
        */
        U2D = 0x3B,

        /*
        INSTRUCTION: F2I

        PURPOSE:
            Convert a 32-bit float to a 64-bit signed integer.

        SEMANTICS:
            REG[DST] = IEEE754_binary32_to_s64(REG[VALUE])

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            VALUE:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1

            LAYOUT:
                [PREFIX*] [F2I] [DST] [VALUE]

        NOTES:
            - Fractions are discarded (round toward zero).
            - Values greater than LONG_MAX are clamped to LONG_MAX.
            - Values less than LONG_MIN are clamped to LONG_MIN.
            - +Infinity is converted to LONG_MAX.
            - -Infinity is converted to LONG_MIN.
            - NaN is converted to 0.
            - +0.0 and -0.0 both convert to 0.
        */
        F2I = 0x3C,

        /*
        INSTRUCTION: F2U

        PURPOSE:
            Convert a 32-bit float to a 64-bit unsigned integer.

        SEMANTICS:
            REG[DST] = IEEE754_binary32_to_u64(REG[VALUE])

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            VALUE:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1

            LAYOUT:
                [PREFIX*] [F2U] [DST] [VALUE]

        NOTES:
            - Fractions are discarded (round toward zero).
            - Values greater than ULONG_MAX are clamped to ULONG_MAX.
            - Values less than ULONG_MIN are clamped to ULONG_MIN.
            - +Infinity is converted to ULONG_MAX.
            - -Infinity is converted to ULONG_MIN.
            - NaN is converted to 0.
            - +0.0 and -0.0 both convert to 0.
        */
        F2U = 0x3D,

        /*
        INSTRUCTION: D2I

        PURPOSE:
            Convert a 64-bit float to a 64-bit signed integer.

        SEMANTICS:
            REG[DST] = IEEE754_binary64_to_s64(REG[VALUE])

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            VALUE:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1

            LAYOUT:
                [PREFIX*] [D2I] [DST] [VALUE]

        NOTES:
            - Fractions are discarded (round toward zero).
            - Values greater than LONG_MAX are clamped to LONG_MAX.
            - Values less than LONG_MIN are clamped to LONG_MIN.
            - +Infinity is converted to LONG_MAX.
            - -Infinity is converted to LONG_MIN.
            - NaN is converted to 0.
            - +0.0 and -0.0 both convert to 0.
        */
        D2I = 0x3E,

        /*
        INSTRUCTION: D2I

        PURPOSE:
            Convert a 64-bit float to a 64-bit unsigned integer.

        SEMANTICS:
            REG[DST] = IEEE754_binary64_to_s64(REG[VALUE])

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            VALUE:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1

            LAYOUT:
                [PREFIX*] [F2I] [DST] [VALUE]

        NOTES:
            - Fractions are discarded (round toward zero).
            - Values greater than LONG_MAX are clamped to LONG_MAX.
            - Values less than LONG_MIN are clamped to LONG_MIN.
            - +Infinity is converted to LONG_MAX.
            - -Infinity is converted to LONG_MIN.
            - NaN is converted to 0.
            - +0.0 and -0.0 both convert to 0.
        */
        D2U = 0x3F,

        /*
        INSTRUCTION: F2D

        PURPOSE:
            Convert a 32-bit float to a 64-bit float.

        SEMANTICS:
            REG[DST] = IEEE754_binary32_to_binary64(REG[VALUE])

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            VALUE:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1

            LAYOUT:
                [PREFIX*] [F2D] [DST] [VALUE]

        NOTES:
            - Everything is preserved.
            - Overflow not possible.
        */
        F2D = 0x40,

        /*
        INSTRUCTION: D2F

        PURPOSE:
            Convert a 64-bit float to a 32-bit float.

        SEMANTICS:
            REG[DST] = IEEE754_binary64_to_binary32(REG[VALUE])

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            VALUE:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1

            LAYOUT:
                [PREFIX*] [D2F] [DST] [VALUE]

        NOTES:
            - Rounded to nearest representable binary32 value (ties to even).
            - Overflow is converted to +-Infinity.
            - NaN is preserved.
        */
        D2F = 0x41,

        /*
        INSTRUCTION: ICMP

        PURPOSE:
            Compare two signed values and produce a result equal to -1, 0 or 1 based on whether lhs is less than, equal to or greater than rhs.

        SEMANTICS:
            let lhs = REG[LHS]
            let rhs = REG[RHS]

            if lhs < rhs: REG[DST] = -1
            else if lhs > rhs: REG[DST] = 1
            else: REG[DST] = 0

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            LHS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

            RHS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND2: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1
                WIDE_OPERAND2

            LAYOUT:
                [PREFIX*] [ICMP] [DST] [LHS] [RHS]
        */
        ICMP = 0x42,

        /*
        INSTRUCTION: UCMP

        PURPOSE:
            Compare two unsigned values and produce a result equal to -1, 0 or 1 based on whether lhs is less than, equal to or greater than rhs.

        SEMANTICS:
            let lhs = REG[LHS]
            let rhs = REG[RHS]

            if lhs < rhs: REG[DST] = -1
            else if lhs > rhs: REG[DST] = 1
            else: REG[DST] = 0

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            LHS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

            RHS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND2: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1
                WIDE_OPERAND2

            LAYOUT:
                [PREFIX*] [UCMP] [DST] [LHS] [RHS]
        */
        UCMP = 0x43,

        /*
        INSTRUCTION: FCMP

        PURPOSE:
            Compare two float values and produce an integer result equal to -1, 0 or 1 based on whether lhs is less than, equal to or greater than rhs.

        SEMANTICS:
            let lhs = REG[LHS]
            let rhs = REG[RHS]

            if lhs < rhs: REG[DST] = -1
            else if lhs > rhs: REG[DST] = 1
            else: REG[DST] = 0

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            LHS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

            RHS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND2: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1
                WIDE_OPERAND2

            LAYOUT:
                [PREFIX*] [FCMP] [DST] [LHS] [RHS]
        */
        FCMP = 0x44,

        /*
        INSTRUCTION: STRCMP

        PURPOSE:
            Compare 2 string objects and produces the value 0 if they are equal,
            a value less than 0 if lhs is lexicographically less than rhs
            and a value greater than 0 if lhs is lexicographically greater than rhs.

        SEMANTICS:
            let lhs = REG[LHS]
            let rhs = REG[RHS]

            let iteratorA = UTF-8 iterator of lhs
            let iteratorB = UTF-8 iterator of rhs

            while iteratorA has next && iteratorB has next:
                let codepointA = next UTF-8 character from iteratorA
                let codepointB = next UTF-8 character from iteratorB

                advance iteratorA
                advance iteratorB

                if codepointA != codepointB:
                    if codepointA < codepointB:
                        return -1
                    else:
                        return 1

            if iteratorA has ended && iteratorB has ended:
                return 0

            if iteratorA not ended:
                return 1
            else:
                return -1

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            LHS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

            RHS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND2: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1
                WIDE_OPERAND2

            LAYOUT:
                [PREFIX*] [STRCMP] [DST] [LHS] [RHS]
        */
        STRCMP = 0x45,

        /*
        INSTRUCTION: INC

        PURPOSE:
            Increment a register value by an immediate value

        SEMANTICS:
            REG[VALUE] += IMM

        OPERANDS:
            VALUE:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            IMM:
                TYPE: immediate
                SIZE:
                    GIGANTIC_IMMEDIATE: 64 bits
                    HUGE_IMMEDIATE: 32 bits
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1
                HUGE_IMMEDIATE
                GIGANTIC_IMMEDIATE

            LAYOUT:
                [PREFIX*] [INC] [VALUE] [IMM]
        */
        INC = 0x46,

        /*
        INSTRUCTION: DEC

        PURPOSE:
            Decrement a register value by an immediate value

        SEMANTICS:
            REG[VALUE] -= IMM

        OPERANDS:
            VALUE:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            IMM:
                TYPE: immediate
                SIZE:
                    GIGANTIC_IMMEDIATE: 64 bits
                    HUGE_IMMEDIATE: 32 bits
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1
                HUGE_IMMEDIATE
                GIGANTIC_IMMEDIATE

            LAYOUT:
                [PREFIX*] [DEC] [VALUE] [IMM]
        */
        DEC = 0x47,

        /*
        INSTRUCTION: JMP

        PURPOSE:
            Increment the instruction pointer by a given signed immediate value.

        SEMANTICS:
            InstructionPointer += BRANCH

        OPERANDS:
            BRANCH:
                TYPE: immediate
                SIZE:
                    GIGANTIC_IMMEDIATE: 64 bits
                    HUGE_IMMEDIATE: 32 bits
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                HUGE_IMMEDIATE
                GIGANTIC_IMMEDIATE

            LAYOUT:
                [PREFIX*] [JMP] [BRANCH]
        */
        JMP = 0x50,

        /*
        INSTRUCTION: JEQ

        PURPOSE:
            Increment the instruction pointer by a given signed immediate value if the value is equal to 0.

        SEMANTICS:
            if REG[VALUE] == 0:
                InstructionPointer += BRANCH

        OPERANDS:
            VALUE:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            BRANCH:
                TYPE: immediate
                SIZE:
                    GIGANTIC_IMMEDIATE: 64 bits
                    HUGE_IMMEDIATE: 32 bits
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1
                HUGE_IMMEDIATE
                GIGANTIC_IMMEDIATE

            LAYOUT:
                [PREFIX*] [JEQ] [VALUE] [BRANCH]
        */
        JEQ = 0x51,

        /*
        INSTRUCTION: JNE

        PURPOSE:
            Increment the instruction pointer by a given signed immediate value if the value isn't equal to 0.

        SEMANTICS:
            if REG[VALUE] != 0:
                InstructionPointer += BRANCH

        OPERANDS:
            VALUE:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            BRANCH:
                TYPE: immediate
                SIZE:
                    GIGANTIC_IMMEDIATE: 64 bits
                    HUGE_IMMEDIATE: 32 bits
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1
                HUGE_IMMEDIATE
                GIGANTIC_IMMEDIATE

            LAYOUT:
                [PREFIX*] [JNE] [VALUE] [BRANCH]
        */
        JNE = 0x52,

        /*
        INSTRUCTION: JLT

        PURPOSE:
            Increment the instruction pointer by a given signed immediate value if the value is less than 0.

        SEMANTICS:
            if REG[VALUE] < 0:
                InstructionPointer += BRANCH

        OPERANDS:
            VALUE:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            BRANCH:
                TYPE: immediate
                SIZE:
                    GIGANTIC_IMMEDIATE: 64 bits
                    HUGE_IMMEDIATE: 32 bits
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1
                HUGE_IMMEDIATE
                GIGANTIC_IMMEDIATE

            LAYOUT:
                [PREFIX*] [JLT] [VALUE] [BRANCH]
        */
        JLT = 0x53,

        /*
        INSTRUCTION: JLE

        PURPOSE:
            Increment the instruction pointer by a given signed immediate value if the value is less than or equal to 0.

        SEMANTICS:
            if REG[VALUE] <= 0:
                InstructionPointer += BRANCH

        OPERANDS:
            VALUE:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            BRANCH:
                TYPE: immediate
                SIZE:
                    GIGANTIC_IMMEDIATE: 64 bits
                    HUGE_IMMEDIATE: 32 bits
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1
                HUGE_IMMEDIATE
                GIGANTIC_IMMEDIATE

            LAYOUT:
                [PREFIX*] [JLE] [VALUE] [BRANCH]
        */
        JLE = 0x54,

        /*
        INSTRUCTION: JGT

        PURPOSE:
            Increment the instruction pointer by a given signed immediate value if the value is greater than 0.

        SEMANTICS:
            if REG[VALUE] > 0:
                InstructionPointer += BRANCH

        OPERANDS:
            VALUE:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            BRANCH:
                TYPE: immediate
                SIZE:
                    GIGANTIC_IMMEDIATE: 64 bits
                    HUGE_IMMEDIATE: 32 bits
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1
                HUGE_IMMEDIATE
                GIGANTIC_IMMEDIATE

            LAYOUT:
                [PREFIX*] [JGT] [VALUE] [BRANCH]
        */
        JGT = 0x55,

        /*
        INSTRUCTION: JGE

        PURPOSE:
            Increment the instruction pointer by a given signed immediate value if the value is greater than or equal to 0.

        SEMANTICS:
            if REG[VALUE] >= 0:
                InstructionPointer += BRANCH

        OPERANDS:
            VALUE:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            BRANCH:
                TYPE: immediate
                SIZE:
                    GIGANTIC_IMMEDIATE: 64 bits
                    HUGE_IMMEDIATE: 32 bits
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1
                HUGE_IMMEDIATE
                GIGANTIC_IMMEDIATE

            LAYOUT:
                [PREFIX*] [JGE] [VALUE] [BRANCH]
        */
        JGE = 0x56,

        // 4 opcodes reserved for future switch instructions with different strategies.
        RESERVED_FOR_SWITCH_0 = 0x57,
        RESERVED_FOR_SWITCH_1 = 0x58,
        RESERVED_FOR_SWITCH_2 = 0x59,
        RESERVED_FOR_SWITCH_3 = 0x5A,

        /*
        INSTRUCTION: NEWINSTANCE

        PURPOSE:
            Dynamically allocate a new uninitialized instance object with the class specified in the constant pool.

        SEMANTICS:
            REG[DST] = internals.AllocateObject(CONST[IDX].classinfo.size())

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            IDX:
                TYPE: const-pool index
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1

            LAYOUT:
                [PREFIX*] [NEWINSTANCE] [DST] [IDX]

        NOTES:
            - Uninitialized can mean the object data is either zeroed out or garbage data.
        */
        NEWINSTANCE = 0x60,

        /*
        INSTRUCTION: NEWARRAY

        PURPOSE:
            Dynamically allocate a new zero-initialized array object with the given type id and length provided.

        SEMANTICS:
            let size = internals.TypeSize(typeid) * REG[LENGTH
            let arr = internals.AllocateObject(size)
            memset(arr.elements, 0, size)
            REG[DST] = arr

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            LENGTH:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

            TYPEID:
                TYPE: immediate
                SIZE: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1

            LAYOUT:
                [PREFIX*] [NEWARRAY] [DST] [LENGTH] [TYPEID]
        */
        NEWARRAY = 0x61,

        /*
        INSTRUCTION: NEWSTRING

        PURPOSE:
            Dynamically allocate a new string object with its UTF-8 characters copied from the byte array containing the raw UTF-8 bytes.

        SEMANTICS:
            let str = internals.AllocateObject(REG[DATA].length)
            memcpy(str, REG[DATA].elements, REG[DATA].length)
            REG[DST] = str

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            DATA:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1

            LAYOUT:
                [PREFIX*] [NEWSTRING] [DST] [DATA]
        */
        NEWSTRING = 0x62,

        /*
        INSTRUCTION: NEWARRAY

        PURPOSE:
            Dynamically allocate a new uncompleted future object.

        SEMANTICS:
            REG[DST] = internals.AllocateObject(sizeof(Future))

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0

            LAYOUT:
                [PREFIX*] [NEWFUTURE] [DST]

        NOTES:
            - This is almost never needed as ASYNC/AWAIT type instructions will generate futures automatically.
        */
        NEWFUTURE = 0x63,

        /*
        INSTRUCTION: CALL

        PURPOSE:
            Call a function found in the constant entry specified with the arguments in the register range starting at the given argument register and continuing right and move its returned value into the specified destination register.

        SEMANTICS:
            REG[DST] = CONST[IDX].call(REG[ARGS]...)

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            IDX:
                TYPE: const-pool index
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

            ARGS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND2: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1
                WIDE_OPERAND2

            LAYOUT:
                [PREFIX*] [CALL] [DST] [IDX] [ARGS]

        NOTES:
            - If the function returns void, possible garbage will still be moved into the destination register.
        */
        CALL = 0xC0,

        /*
        INSTRUCTION: TAIL_CALL

        PURPOSE:
            Prepare the current stack frame for running the specified function (if needed)
            and move the specified argument range down to 0, then transfer control over to the function.

        SEMANTICS:
            memmove(REG[0], REG[ARGS], CONST[IDX].paramCount)
            InstructionPointer = CONST[IDX].entry

        OPERANDS:
            IDX:
                TYPE: const-pool index
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            ARGS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1

            LAYOUT:
                [PREFIX*] [TAIL_CALL] [IDX] [ARGS]

        NOTES:
            - As this instruction transfers control without creating a new frame, all following instructions are
              unreachable
        */
        TAIL_CALL = 0xC1,

        /*
        INSTRUCTION: CALLA

        PURPOSE:
            Call a function found in the constant entry specified asynchronously with the same priority of the currently executing task with the arguments in the register range starting at the given argument register and continuing right and move a new future to the specified destination register which will resolve to the returned value of the function once it returns.

        SEMANTICS:
            REG[DST] = CONST[IDX].callAsync(...REG[ARGS], CurrentTask.priority)

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            IDX:
                TYPE: const-pool index
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

            ARGS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND2: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1
                WIDE_OPERAND2

            LAYOUT:
                [PREFIX*] [CALLA] [DST] [IDX] [ARGS]
        */
        CALLA = 0xC2,

        /*
        INSTRUCTION: CALLAP

        PURPOSE:
            Call a function found in the constant entry specified asynchronously with the specified priority
            with the arguments in the register range starting at the given argument register and continuing
            right and move a new future to the specified destination register which will resolve to the returned
            value of the function once it returns.

        SEMANTICS:
            REG[DST] = CONST[IDX].callAsync(REG[ARGS]..., REG[PRIORITY] & 0xFF)

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            PRIORITY:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

            IDX:
                TYPE: const-pool index
                SIZE:
                    WIDE_OPERAND2: 16 bits
                    DEFAULT: 8 bits

            ARGS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND3: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1
                WIDE_OPERAND2
                WIDE_OPERAND3

            LAYOUT:
                [PREFIX*] [CALLAP] [DST] [PRIORITY] [IDX] [ARGS]

        NOTES:
            - The priority register must hold an unsigned value which is then truncated down to 8 bits.
        */
        CALLAP = 0xC3,

        /*
        INSTRUCTION: CALLARP

        PURPOSE:
            Call a function found in the constant entry specified asynchronously with the current
            executing tasks priority plus the specified priority as a signed value with the arguments
            in the register range starting at the given argument register and continuing right and move
            a new future to the specified destination register which will resolve to the
            returned value of the function once it returns.

        SEMANTICS:
            REG[DST] = CONST[IDX].callAsync(REG[ARGS]..., CurrentTask.priority + PRIORITY)

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            PRIORITY:
                TYPE: immediate
                SIZE: 8 bits

            IDX:
                TYPE: const-pool index
                SIZE:
                    WIDE_OPERAND2: 16 bits
                    DEFAULT: 8 bits

            ARGS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND3: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1
                WIDE_OPERAND2
                WIDE_OPERAND3

            LAYOUT:
                [PREFIX*] [CALLARP] [DST] [PRIORITY] [IDX] [ARGS]
        */
        CALLARP = 0xC4,

        /*
        INSTRUCTION: CALL_DYN

        PURPOSE:
            Call a function found in the specified register with the arguments in the register range starting at the given argument register and continuing right and move its returned value into the specified destination register.

        SEMANTICS:
            REG[DST] = REG[FN].call(REG[ARGS]...)

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            FN:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

            ARGS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND2: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1
                WIDE_OPERAND2

            LAYOUT:
                [PREFIX*] [CALL_DYN] [DST] [FN] [ARGS]

        NOTES:
            - If the function returns void, possible garbage will still be moved into the destination register.
        */
        CALL_DYN = 0xC5,

        /*
        INSTRUCTION: TAIL_CALL_DYN

        PURPOSE:
            Prepare the current stack frame for running the specified function (if needed)
            and move the specified argument range down to 0, then transfer control over to the function.

        SEMANTICS:
            memmove(REG[0], REG[ARGS], REG[FN].paramCount)
            InstructionPointer = CONST[IDX].entry

        OPERANDS:
            FN:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

            ARGS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1

            LAYOUT:
                [PREFIX*] [TAIL_CALL_DYN] [FN] [ARGS]

        NOTES:
            - As this instruction transfers control without creating a new frame, all following instructions are
              unreachable
        */
        TAIL_CALL_DYN = 0xC6,

        /*
        INSTRUCTION: CALLA_DYN

        PURPOSE:
            Call a function found in the specified register asynchronously with the same priority of the currently executing task with the arguments in the register range starting at the given argument register and continuing right and move a new future to the specified destination register which will resolve to the returned value of the function once it returns.

        SEMANTICS:
            REG[DST] = REG[FN].callAsync(...REG[ARGS], CurrentTask.priority)

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            FN:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

            ARGS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND2: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1
                WIDE_OPERAND2

            LAYOUT:
                [PREFIX*] [CALLA_DYN] [DST] [FN] [ARGS]
        */
        CALLA_DYN = 0xC7,

        /*
        INSTRUCTION: CALLAP_DYN

        PURPOSE:
            Call a function found in the specified register asynchronously with the specified priority
            with the arguments in the register range starting at the given argument register and continuing
            right and move a new future to the specified destination register which will resolve to the returned
            value of the function once it returns.

        SEMANTICS:
            REG[DST] = REG[FN].callAsync(REG[ARGS]..., REG[PRIORITY] & 0xFF)

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            PRIORITY:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

            FN:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

            ARGS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND3: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1
                WIDE_OPERAND2
                WIDE_OPERAND3

            LAYOUT:
                [PREFIX*] [CALLAP_DYN] [DST] [PRIORITY] [FN] [ARGS]

        NOTES:
            - The priority register must hold an unsigned value which is then truncated down to 8 bits.
        */
        CALLAP_DYN = 0xC8,

        /*
        INSTRUCTION: CALLARP_DYN

        PURPOSE:
            Call a function found in the specified register asynchronously with the current
            executing tasks priority plus the specified priority as a signed value with the arguments
            in the register range starting at the given argument register and continuing right and move
            a new future to the specified destination register which will resolve to the
            returned value of the function once it returns.

        SEMANTICS:
            REG[DST] = REG[FN].callAsync(REG[ARGS]..., CurrentTask.priority + PRIORITY)

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            PRIORITY:
                TYPE: immediate
                SIZE: 8 bits

            FN:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

            ARGS:
                TYPE: register
                SIZE:
                    WIDE_OPERAND3: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1
                WIDE_OPERAND2
                WIDE_OPERAND3

            LAYOUT:
                [PREFIX*] [CALLARP_DYN] [DST] [PRIORITY] [FN] [ARGS]
        */
        CALLARP_DYN = 0xC9,

        /*
        INSTRUCTION: RETURN

        PURPOSE:
            Return from the current function with a provided value.

        SEMANTICS:
            return REG[VALUE]

        OPERANDS:
            VALUE:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0

            LAYOUT:
                [PREFIX*] [RETURN] [VALUE]
        */
        RETURN = 0xCA,

        /*
        INSTRUCTION: RETURN

        PURPOSE:
            Return from the current function with a provided value.

        SEMANTICS:
            if REG[FUTURE].ready():
                REG[DST] = REG[FUTURE].value

        OPERANDS:
            DST:
                TYPE: register
                SIZE:
                    WIDE_OPERAND0: 16 bits
                    DEFAULT: 8 bits

            FUTURE:
                TYPE: register
                SIZE:
                    WIDE_OPERAND1: 16 bits
                    DEFAULT: 8 bits

        ENCODING:
            PREFIXES:
                WIDE_OPERAND0
                WIDE_OPERAND1

            LAYOUT:
                [PREFIX*] [RETURN] [DST] [FUTURE]
        */
        AWAIT = 0xCB,

        /*
        INSTRUCTION: RETURN

        PURPOSE:
            Yield the current executing task early, letting the next one in queue execute.

        SEMANTICS:
            yield()

        OPERANDS:
            none

        ENCODING:
            PREFIXES:
                none

            LAYOUT:
                [PREFIX*] [YIELD]

        NOTES:
            - With how priority is implemented, there's a chance that the task will begin executing immediately again. I do not know yet if that is the case, but I will update this once I figure it out.
        */
        YIELD = 0xCC,
    };

    namespace opcodeutils {
        struct PrefixState {
            bool wideOperand0 = false;
            bool wideOperand1 = false;
            bool wideOperand2 = false;
            bool wideOperand3 = false;
            bool hugeImmediate = false;
            bool giganticImmediate = false;
        };

        // Including opcode. 0 means variable length, which needs a bit more context to determine size.
        size_t GetFixedLength(Opcode opcode, const PrefixState& prefixState);
    }
}

#endif //BIBBLEVM_OPCODES_H