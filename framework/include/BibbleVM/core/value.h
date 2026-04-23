// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_CORE_VALUE_H
#define BIBBLEVM_CORE_VALUE_H 1

#include "BibbleVM/core/oop/type.h"

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

        ::Value toNative() const {
            ::Value out{};
            out.b = b;
            out.ub = ub;
            out.s = s;
            out.us = us;
            out.i = i;
            out.ui = ui;
            out.l = l;
            out.ul = ul;
            out.f = f;
            out.d = d;
            out.h = h;
            out.obj = reinterpret_cast<VMObject>(obj);
            return out;
        }
    };
}

#endif //BIBBLEVM_CORE_VALUE_H