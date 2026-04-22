// Copyright 2026 JesusTouchMe

#include "BibbleVM/core/opcodes.h"

namespace bibblevm::opcodeutils {
    static size_t GetRegisterSize(bool wide) {
        return wide ? 2 : 1;
    }

    static size_t GetImmediateSize(bool wide, bool huge = false, bool gigantic = false) {
        if (gigantic) return 8;
        if (huge) return 4;
        if (wide) return 2;
        return 1;
    }

    static size_t GetConstPoolIndexSize(bool wide) {
        return wide ? 2 : 1;
    }

    size_t GetFixedLength(Opcode opcode, const PrefixState& prefixState) {
        size_t size = 1;
        if (prefixState.wideOperand0) size++;
        if (prefixState.wideOperand1) size++;
        if (prefixState.wideOperand2) size++;
        if (prefixState.wideOperand3) size++;
        if (prefixState.hugeImmediate) size++;
        if (prefixState.giganticImmediate) size++;

        switch (static_cast<Opcodes>(opcode)) {
            case NOP:
                break;
            case MOV:
                size += GetRegisterSize(prefixState.wideOperand0);
                size += GetRegisterSize(prefixState.wideOperand1);
                break;
            case MOV_RANGE:
                size += GetRegisterSize(prefixState.wideOperand0);
                size += GetRegisterSize(prefixState.wideOperand1);
                size += GetImmediateSize(prefixState.wideOperand2);
                break;
            case SWAP:
                size += GetRegisterSize(prefixState.wideOperand0);
                size += GetRegisterSize(prefixState.wideOperand1);
                break;
            case LOAD_CONST:
                size += GetRegisterSize(prefixState.wideOperand0);
                size += GetConstPoolIndexSize(prefixState.wideOperand1);
                break;
            case LOAD_IMM:
                size += GetRegisterSize(prefixState.wideOperand0);
                size += GetImmediateSize(prefixState.wideOperand1, prefixState.hugeImmediate, prefixState.giganticImmediate);
                break;
            case ADD:
            case SUB:
            case MUL:
            case SDIV:
            case UDIV:
            case SMOD:
            case UMOD:
                size += GetRegisterSize(prefixState.wideOperand0);
                size += GetRegisterSize(prefixState.wideOperand1);
                size += GetRegisterSize(prefixState.wideOperand2);
                break;
            case NEG:
            case ABS:
                size += GetRegisterSize(prefixState.wideOperand0);
                size += GetRegisterSize(prefixState.wideOperand1);
                break;
            case AND:
            case OR:
            case XOR:
                size += GetRegisterSize(prefixState.wideOperand0);
                size += GetRegisterSize(prefixState.wideOperand1);
                size += GetRegisterSize(prefixState.wideOperand2);
                break;
            case NOT:
                size += GetRegisterSize(prefixState.wideOperand0);
                size += GetRegisterSize(prefixState.wideOperand1);
                break;
            case SHL:
            case SHR:
            case SAR:
            case FADD:
            case FSUB:
            case FMUL:
            case FDIV:
                size += GetRegisterSize(prefixState.wideOperand0);
                size += GetRegisterSize(prefixState.wideOperand1);
                size += GetRegisterSize(prefixState.wideOperand2);
                break;
            case FNEG:
            case FABS:
            case TR8:
            case TR16:
            case TR32:
            case SEX8:
            case SEX16:
            case SEX32:
            case ZEX8:
            case ZEX16:
            case ZEX32:
            case I2F:
            case U2F:
            case I2D:
            case U2D:
            case F2I:
            case F2U:
            case D2I:
            case D2U:
            case F2D:
            case D2F:
                size += GetRegisterSize(prefixState.wideOperand0);
                size += GetRegisterSize(prefixState.wideOperand1);
                break;
            case ICMP:
            case UCMP:
            case FCMP:
            case STRCMP:
                size += GetRegisterSize(prefixState.wideOperand0);
                size += GetRegisterSize(prefixState.wideOperand1);
                size += GetRegisterSize(prefixState.wideOperand2);
                break;
            case INC:
            case DEC:
                size += GetRegisterSize(prefixState.wideOperand0);
                size += GetImmediateSize(prefixState.wideOperand1, prefixState.hugeImmediate, prefixState.giganticImmediate);
                break;
            case JMP:
                size += GetImmediateSize(prefixState.wideOperand0, prefixState.hugeImmediate, prefixState.giganticImmediate);
                break;
            case JEQ:
            case JNE:
            case JLT:
            case JLE:
            case JGT:
            case JGE:
                size += GetRegisterSize(prefixState.wideOperand0);
                size += GetImmediateSize(prefixState.wideOperand1, prefixState.hugeImmediate, prefixState.giganticImmediate);
                break;
            case RESERVED_FOR_SWITCH_0:
            case RESERVED_FOR_SWITCH_1:
            case RESERVED_FOR_SWITCH_2:
            case RESERVED_FOR_SWITCH_3:
                break;
            case NEWINSTANCE:
                size += GetRegisterSize(prefixState.wideOperand0);
                size += GetConstPoolIndexSize(prefixState.wideOperand1);
                break;
            case NEWARRAY:
                size += GetRegisterSize(prefixState.wideOperand0);
                size += GetRegisterSize(prefixState.wideOperand1);
                size += 1;
                break;
            case NEWSTRING:
                size += GetRegisterSize(prefixState.wideOperand0);
                size += GetRegisterSize(prefixState.wideOperand1);
                break;
            case NEWFUTURE:
                size += GetRegisterSize(prefixState.wideOperand0);
                break;
            case CALL:
                size += GetRegisterSize(prefixState.wideOperand0);
                size += GetConstPoolIndexSize(prefixState.wideOperand1);
                size += GetRegisterSize(prefixState.wideOperand2);
                break;
            case TAIL_CALL:
                size += GetConstPoolIndexSize(prefixState.wideOperand0);
                size += GetRegisterSize(prefixState.wideOperand1);
                break;
            case CALLA:
                size += GetRegisterSize(prefixState.wideOperand0);
                size += GetConstPoolIndexSize(prefixState.wideOperand1);
                size += GetRegisterSize(prefixState.wideOperand2);
                break;
            case CALLAP:
                size += GetRegisterSize(prefixState.wideOperand0);
                size += GetRegisterSize(prefixState.wideOperand1);
                size += GetConstPoolIndexSize(prefixState.wideOperand2);
                size += GetRegisterSize(prefixState.wideOperand3);
                break;
            case CALLARP:
                size += GetRegisterSize(prefixState.wideOperand0);
                size += 1;
                size += GetConstPoolIndexSize(prefixState.wideOperand1);
                size += GetRegisterSize(prefixState.wideOperand2);
                break;
            case CALL_DYN:
                size += GetRegisterSize(prefixState.wideOperand0);
                size += GetRegisterSize(prefixState.wideOperand1);
                size += GetRegisterSize(prefixState.wideOperand2);
                break;
            case TAIL_CALL_DYN:
                size += GetRegisterSize(prefixState.wideOperand0);
                size += GetRegisterSize(prefixState.wideOperand1);
                break;
            case CALLA_DYN:
                size += GetRegisterSize(prefixState.wideOperand0);
                size += GetRegisterSize(prefixState.wideOperand1);
                size += GetRegisterSize(prefixState.wideOperand2);
                break;
            case CALLAP_DYN:
                size += GetRegisterSize(prefixState.wideOperand0);
                size += GetRegisterSize(prefixState.wideOperand1);
                size += GetRegisterSize(prefixState.wideOperand2);
                size += GetRegisterSize(prefixState.wideOperand3);
                break;
            case CALLARP_DYN:
                size += GetRegisterSize(prefixState.wideOperand0);
                size += 1;
                size += GetRegisterSize(prefixState.wideOperand1);
                size += GetRegisterSize(prefixState.wideOperand2);
                break;
            case RETURN:
                size += GetRegisterSize(prefixState.wideOperand0);
                break;
            case AWAIT:
                size += GetRegisterSize(prefixState.wideOperand0);
                size += GetRegisterSize(prefixState.wideOperand1);
                break;
            case YIELD:
                break;
        }

        return size;
    }

    bool IsBranch(Opcode opcode) {
        return opcode >= JMP && opcode <= JGE;
    }
}
