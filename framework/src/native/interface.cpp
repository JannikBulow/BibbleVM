// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/native/interface.h"

#include "BibbleVM/vm.h"

namespace bibblevm::native {
    static VM& GetVM(BibbleVM* vm) {
        return *reinterpret_cast<VM*>(vm);
    }

    static executor::Module* GetModule(VMModule module) {
        return reinterpret_cast<executor::Module*>(module);
    }

    static oop::Class* GetClass(VMClass clas) {
        return reinterpret_cast<oop::Class*>(clas);
    }

    static const oop::Field* GetField(VMField field) {
        return reinterpret_cast<const oop::Field*>(field);
    }

    static const oop::Method* GetMethod(VMMethod method) {
        return reinterpret_cast<const oop::Method*>(method);
    }

    static executor::Function* GetFunction(VMFunction function) {
        return reinterpret_cast<executor::Function*>(function);
    }

    static oop::Object* GetObject(VMObject object) {
        return reinterpret_cast<oop::Object*>(object);
    }

    VMModule GetModule(BibbleVM* vm_, const char* name) {
        VM& vm = GetVM(vm_);
        linker::Module* module = vm.getModule(name);
        return reinterpret_cast<VMModule>(&module->linkedModule());
    }

    VMObject NewGlobalReference(BibbleVM* vm_, VMObject object) {
        VM& vm = GetVM(vm_);
        oop::Object** reference = vm.memoryManager().newGlobalStrongReference(reinterpret_cast<oop::Object*>(object));
        return reinterpret_cast<VMObject>(reference);
    }

    void DeleteGlobalReference(BibbleVM* vm_, VMObject object) {
        VM& vm = GetVM(vm_);
        vm.memoryManager().deleteGlobalStrongReference(reinterpret_cast<oop::Object**>(object));
    }

    VMObject NewLocalReference(BibbleVM* vm_, VMObject object) {
        VM& vm = GetVM(vm_);
        oop::Object** reference = vm.memoryManager().newLocalStrongReference(reinterpret_cast<oop::Object*>(object));
        return reinterpret_cast<VMObject>(reference);
    }

    void PushLocalReferenceFrame(BibbleVM* vm_, VMUInt initialSize) {
        VM& vm = GetVM(vm_);
        vm.memoryManager().pushLocalReferenceFrame(initialSize);
    }

    void PopLocalReferenceFrame(BibbleVM* vm_) {
        VM& vm = GetVM(vm_);
        vm.memoryManager().popLocalReferenceFrame();
    }

    VMClass GetClass(BibbleVM* vm_, VMModule module_, const char* name) {
        VM& vm = GetVM(vm_);
        executor::Module* module = GetModule(module_);
        if (module == nullptr) return nullptr;
        return reinterpret_cast<VMClass>(module->getClass(name));
    }

    VMClass GetSuperClass(BibbleVM* vm_, VMClass clas_) {
        VM& vm = GetVM(vm_);
        oop::Class* clas = GetClass(clas_);
        if (clas == nullptr) return nullptr;
        return reinterpret_cast<VMClass>(clas->getSuperClass());
    }

    VMBool IsAssignableFrom(BibbleVM* vm_, VMClass a_, VMClass b_) {
        VM& vm = GetVM(vm_);
        oop::Class* a = GetClass(a_);
        oop::Class* b = GetClass(b_);
        if (a == nullptr || b == nullptr) return false;
        return a->isAssignableTo(b) ? BIBBLE_TRUE : BIBBLE_FALSE; // This is technically inefficient, but compiler should optimize it away. It's more important that we explicitly show semantics and shi.. on some smart pack
    }

    VMField GetField(BibbleVM* vm_, VMClass clas_, const char* name) {
        VM& vm = GetVM(vm_);
        oop::Class* clas = GetClass(clas_);
        if (clas == nullptr) return nullptr;
        return reinterpret_cast<VMField>(clas->getField(name));
    }

    VMValue GetFieldValue(BibbleVM* vm_, VMObject object_, VMField field_) {
        VM& vm = GetVM(vm_);
        oop::Object* object = GetObject(object_);
        const oop::Field* field = GetField(field_);
        if (object == nullptr || field == nullptr) return {};
        if (object->kind != oop::ObjectKind::Instance) return {}; // TODO: some error case?
        Value value = object->asInstance()->clas->readField(object->asInstance(), field);
        return value.toNative(vm.memoryManager());
    }

    void SetFieldValue(BibbleVM* vm_, VMObject object_, VMField field_, VMValue value) {
        VM& vm = GetVM(vm_);
        oop::Object* object = GetObject(object_);
        const oop::Field* field = GetField(field_);
        if (object == nullptr || field == nullptr) return;
        if (object->kind != oop::ObjectKind::Instance) return;
        Value usableValue = Value::FromNative(value, field->type == oop::Type::Reference);
        object->asInstance()->clas->writeField(object->asInstance(), field, usableValue);
    }

    VMMethod GetMethod(BibbleVM* vm_, VMClass clas_, const char* name) {
        VM& vm = GetVM(vm_);
        oop::Class* clas = GetClass(clas_);
        if (clas == nullptr) return nullptr;
        return reinterpret_cast<VMMethod>(clas->getMethod(name));
    }

    VMFunction DispatchMethod(BibbleVM* vm_, VMObject object_, VMMethod method_) {
        VM& vm = GetVM(vm_);
        oop::Object* object = GetObject(object_);
        const oop::Method* method = GetMethod(method_);
        if (object == nullptr || method == nullptr) return nullptr;
        if (object->kind != oop::ObjectKind::Instance) return nullptr;
        return reinterpret_cast<VMFunction>(object->asInstance()->clas->dispatchMethod(method));
    }

    VMFunction DispatchMethodNonVirtual(BibbleVM* vm_, VMClass clas_, VMMethod method_) {
        VM& vm = GetVM(vm_);
        oop::Class* clas = GetClass(clas_);
        const oop::Method* method = GetMethod(method_);
        if (clas == nullptr || method == nullptr) return nullptr;
        return reinterpret_cast<VMFunction>(clas->dispatchMethod(method));
    }

    VMFunction GetFunction(BibbleVM* vm_, VMModule module_, const char* name) {
        VM& vm = GetVM(vm_);
        executor::Module* module = GetModule(module_);
    }

    void PopulateInterface(BibbleInterface* interface) {
        interface->GetModule = GetModule;
        interface->NewGlobalReference = NewGlobalReference;
        interface->DeleteGlobalReference = DeleteGlobalReference;
        interface->NewLocalReference = NewLocalReference;
        interface->PushLocalReferenceFrame = PushLocalReferenceFrame;
        interface->PopLocalReferenceFrame = PopLocalReferenceFrame;
        interface->GetClass = GetClass;
        interface->GetSuperClass = GetSuperClass;
        interface->IsAssignableFrom = IsAssignableFrom;
        interface->GetField = GetField;
        interface->GetFieldValue = GetFieldValue;
        interface->SetFieldValue = SetFieldValue;
        interface->GetMethod = GetMethod;
        interface->DispatchMethod = DispatchMethod;
        interface->DispatchMethodNonVirtual = DispatchMethodNonVirtual;
    }
}
