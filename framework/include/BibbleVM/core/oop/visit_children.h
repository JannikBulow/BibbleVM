// Copyright 2026 JesusTouchMe

#ifndef BIBBLEVM_CORE_OOP_VISIT_CHILDREN_H
#define BIBBLEVM_CORE_OOP_VISIT_CHILDREN_H 1

#include "BibbleVM/core/oop/class.h"
#include "BibbleVM/core/oop/object.h"

namespace bibblevm::oop {
    constexpr void Object::visitChildren(auto visitor) {
        switch (kind) {
            case ObjectKind::Newborn: break;
            case ObjectKind::Instance: return asInstance()->visitChildren(visitor);
            case ObjectKind::Array: return asArray()->visitChildren(visitor);
            case ObjectKind::String: return asString()->visitChildren(visitor);
            case ObjectKind::Future: return asFuture()->visitChildren(visitor);
        }
    }

    constexpr void Instance::visitChildren(auto visitor) {
        clas->visitReferenceRegions([this, &visitor](const Class::ReferenceRegion& region) {
            Object** children = reinterpret_cast<Object**>(fieldBytes + region.offset);
            for (uint16_t i = 0; i < region.count; i++) {
                visitor(children[i]);
            }
        });
    }

    constexpr void Array::visitChildren(auto visitor) {
        if (baseType == Type::Reference) {
            Object** children = reinterpret_cast<Object**>(elementBytes);
            for (ULong i = 0; i < length; i++) {
                visitor(children[i]);
            }
        }
    }

    constexpr void StringObject::visitChildren(auto visitor) {

    }

    constexpr void Future::visitChildren(auto visitor) {
        visitor(waiters);
        if (ready && value.isObject) {
            visitor(value.obj);
        } else if (cancelled) {
            visitor(error.message);
        }
    }
}

#endif //BIBBLEVM_CORE_OOP_VISIT_CHILDREN_H