// Copyright 2026 JesusTouchMe

#include "BibbleVM/core/opcodes.h"

namespace bibblevm::opcodeutils {
    size_t GetFixedLength(Opcode opcode, const PrefixState& prefixState) {
        switch (static_cast<Opcodes>(opcode)) {
            case NOP: return 1;
            case MOV: return 3;
            case MOV_RANGE: break;
            case SWAP: break;
            case LOAD_CONST: break;
            case LOAD_IMM: break;
            case ADD: break;
            case SUB: break;
            case MUL: break;
            case SDIV: break;
            case UDIV: break;
            case SMOD: break;
            case UMOD: break;
            case NEG: break;
            case ABS: break;
            case AND: break;
            case OR: break;
            case XOR: break;
            case NOT: break;
            case SHL: break;
            case SHR: break;
            case SAR: break;
            case FADD: break;
            case FSUB: break;
            case FMUL: break;
            case FDIV: break;
            case FNEG: break;
            case FABS: break;
            case TR8: break;
            case TR16: break;
            case TR32: break;
            case SEX8: break;
            case SEX16: break;
            case SEX32: break;
            case ZEX8: break;
            case ZEX16: break;
            case ZEX32: break;
            case I2F: break;
            case U2F: break;
            case I2D: break;
            case U2D: break;
            case F2I: break;
            case F2U: break;
            case D2I: break;
            case D2U: break;
            case F2D: break;
            case D2F: break;
            case ICMP: break;
            case UCMP: break;
            case FCMP: break;
            case STRCMP: break;
            case INC: break;
            case DEC: break;
            case JMP: break;
            case JEQ: break;
            case JNE: break;
            case JLT: break;
            case JLE: break;
            case JGT: break;
            case JGE: break;
            case RESERVED_FOR_SWITCH_0: break;
            case RESERVED_FOR_SWITCH_1: break;
            case RESERVED_FOR_SWITCH_2: break;
            case RESERVED_FOR_SWITCH_3: break;
            case NEWINSTANCE: break;
            case NEWARRAY: break;
            case NEWSTRING: break;
            case NEWFUTURE: break;
            case CALL: break;
            case TAIL_CALL: break;
            case CALLA: break;
            case CALLAP: break;
            case CALLARP: break;
            case CALL_DYN: break;
            case TAIL_CALL_DYN: break;
            case CALLA_DYN: break;
            case CALLAP_DYN: break;
            case CALLARP_DYN: break;
            case RETURN: break;
            case AWAIT: break;
            case YIELD: break;

        }
    }
}
