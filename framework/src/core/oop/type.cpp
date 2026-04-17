// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/core/oop/class.h"
#include "BibbleVM/core/oop/type.h"

namespace bibblevm::oop {
    size_t GetPrimitiveSizeForType(Type type) {
        switch (type) {
            case Type::Byte: return 1;
            case Type::UByte: return 1;
            case Type::Short: return 2;
            case Type::UShort: return 2;
            case Type::Int: return 4;
            case Type::UInt: return 4;
            case Type::Long: return 8;
            case Type::ULong: return 8;
            case Type::Float: return 4;
            case Type::Double: return 8;
            case Type::Handle: return 8;
            case Type::Reference: return 8;
            case Type::ModuleRef: return 8;
            case Type::ClassRef: return 8;
            case Type::FieldRef: return 8;
            case Type::MethodRef: return 8;
            case Type::FunctionRef: return 8;
            case Type::Count: return 0;
        }
        return 0;
    }
}
