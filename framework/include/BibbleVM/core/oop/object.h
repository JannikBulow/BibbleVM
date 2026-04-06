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
    struct Instance;
    struct Array;
    struct StringObject;
    struct Future;

    struct Object {
        TypeID type;
        uint32_t allocatedSize;
        Object* forward = nullptr;

        Instance* asInstance() { return reinterpret_cast<Instance*>(this); }
        Array* asArray() { return reinterpret_cast<Array*>(this); }
        StringObject* asString() { return reinterpret_cast<StringObject*>(this); }
        Future* asFuture() { return reinterpret_cast<Future*>(this); }

        // Takes its type explicitly because I want this function inline and I can't include memory_manager.
        constexpr void visitChildren(Type* resolvedType, auto visitor);
    };

    struct Instance {
        Object header;
        bool finalizerQueued = false;
        char fieldBytes[];

        Object* asObject() { return &header; }

        constexpr void visitChildren(Type* resolvedType, auto visitor) {
            Object** children = reinterpret_cast<Object**>(fieldBytes);
            for (uint16_t i = 0; i < resolvedType->getObjectFieldCount(); i++) {
                visitor(children[i]);
            }
        }
    };

    struct Array {
        Object header;
        ULong length;
        char elementBytes[];

        Object* asObject() { return &header; }

        constexpr void visitChildren(Type* resolvedType, auto visitor) {
            if (resolvedType->baseType->isObjectType()) {
                Object** children = reinterpret_cast<Object**>(elementBytes);
                for (ULong i = 0; i < length; i++) {
                    visitor(children[i]);
                }
            }
        }
    };

    struct StringObject {
        Object header;
        ULong lengthBytes;
        char bytes[];

        Object* asObject() { return &header; }

        constexpr void visitChildren(Type* resolvedType, auto visitor) {}
    };

    struct BIBBLEVM_EXPORT Future {
        Object header;

        bool ready = false;
        Value value;

        Object* waiters; // Array with executor::Task* elements
        ULong waiterCount;

        Object* asObject() { return &header; }

        constexpr void visitChildren(Type* resolvedType, auto visitor) {
            visitor(waiters);
        }

        void addWaiter(VM& vm, executor::Task* waiter);
        void complete(VM& vm, Value value);
    };

    constexpr void Object::visitChildren(Type* resolvedType, auto visitor) {
        switch (resolvedType->kind) {
            case Type::Instance:
                asInstance()->visitChildren(resolvedType, visitor);
                break;
            case Type::Array:
                asArray()->visitChildren(resolvedType, visitor);
                break;
            case Type::String:
                asString()->visitChildren(resolvedType, visitor);
                break;
            case Type::Future:
                asFuture()->visitChildren(resolvedType, visitor);
                break;
            default:
                break;
        }
    }
}

#endif // BIBBLEVM_CORE_OOP_OBJECT_H
