// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_CORE_VALUE_H
#define BIBBLEVM_CORE_VALUE_H 1

#include <BibbleInterface.h>

namespace bibblevm {
    using Byte = VMByte;
    using UByte = VMUByte;
    using Short = VMShort;
    using UShort = VMUShort;
    using Int = VMInt;
    using UInt = VMUInt;
    using Long = VMLong;
    using ULong = VMULong;
    using Float = VMFloat;
    using Double = VMDouble;
    using Handle = VMHandle;

    namespace executor {
        class Function;
        class Module;
    }

    namespace gc {
        class MemoryManager;
    }

    namespace oop {
        class Class;
        struct Field;
        struct Method;
        struct Object;
    }

    struct Value {
        bool isObject;
        union {
            Byte b;
            UByte ub;
            Short s;
            UShort us;
            Int i;
            UInt ui;
            Long l;
            ULong ul;
            Float f;
            Double d;
            Handle h;
            oop::Object* obj;
            executor::Module* mi;
            oop::Class* ci;
            oop::Field* fi;
            oop::Method* mei;
            executor::Function* fni;
        };

        static Value FromNative(VMValue v, bool reference) {
            Value out{};
            out.isObject = reference;
            if (reference) {
                out.obj = *reinterpret_cast<oop::Object**>(v.obj);
            } else {
                out.b = v.b;
                out.ub = v.ub;
                out.s = v.s;
                out.us = v.us;
                out.i = v.i;
                out.ui = v.ui;
                out.l = v.l;
                out.ul = v.ul;
                out.f = v.f;
                out.d = v.d;
                out.h = v.h;
            }
            return out;
        }

        // Only use when your native reference frame it set up.
        VMValue toNative(gc::MemoryManager& memoryManager) const;
    };
}

#endif //BIBBLEVM_CORE_VALUE_H