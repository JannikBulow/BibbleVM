// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_CORE_OOP_OBJECT_H
#define BIBBLEVM_CORE_OOP_OBJECT_H 1

#include "BibbleVM/core/oop/type.h"

#include "BibbleVM/core/value.h"

#include "BibbleVM/api.h"

namespace bibblevm {
    class VM;
}

namespace bibblevm::executor {
    struct Task;
}

namespace bibblevm::oop {
    class Class;
    struct Instance;
    struct Array;
    struct StringObject;
    struct Future;

    enum class ObjectKind : uint8_t {
        Newborn, // Objects cannot be this kind after they leave the MemoryManager
        Instance,
        Array,
        String,
        Future,
    };

    struct Object {
        Object* forward = nullptr;
        uint32_t allocatedSize;
        ObjectKind kind;
        uint8_t age = 0;
        uint8_t markBit = 0;
        uint8_t heapID = 0; // 0 is reserved here for unknown heap id

        Instance* asInstance() { return reinterpret_cast<Instance*>(this); }
        Array* asArray() { return reinterpret_cast<Array*>(this); }
        StringObject* asString() { return reinterpret_cast<StringObject*>(this); }
        Future* asFuture() { return reinterpret_cast<Future*>(this); }

        // Takes its type explicitly because I want this function inline and I can't include memory_manager.
        constexpr void visitChildren(auto visitor);
    };

    struct Instance {
        Object header;
        Class* clas;
        char fieldBytes[];

        Object* asObject() { return &header; }

        constexpr void visitChildren(auto visitor);
    };

    struct Array {
        Object header;
        Type baseType;
        ULong length;
        uint8_t elementBytes[];

        Object* asObject() { return &header; }

        constexpr void visitChildren(auto visitor);
    };

    struct StringObject {
        Object header;
        ULong lengthBytes;
        char bytes[];

        Object* asObject() { return &header; }

        constexpr void visitChildren(auto visitor);
    };

    struct BIBBLEVM_EXPORT Future {
        Object header;

        bool ready = false;
        Value value;

        Object* waiters; // Array with executor::Task* elements
        ULong waiterCount;

        Object* asObject() { return &header; }

        constexpr void visitChildren(auto visitor);

        void addWaiter(VM& vm, executor::Task* waiter);
        void complete(VM& vm, Value value);
    };
}

#endif // BIBBLEVM_CORE_OOP_OBJECT_H
