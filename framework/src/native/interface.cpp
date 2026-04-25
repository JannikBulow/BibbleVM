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

    static executor::Task* ExtractTask(BibbleInterface* interface) {
        return static_cast<executor::Task*>(interface->reserved[0]);
    }

    // Shittily execute a function whilst avoiding the scheduler because there's currently no way to do it cleanly.
    static Value ExecuteFunction(VM& vm, executor::Function* function, executor::Task* task, auto addArgs) {
        using namespace executor;

        Stack& stack = task->stack;
        Frame* targetFrame = stack.getTop();

        Value returnValue{};

        Frame& newFrame = stack.pushFrame(*function, &returnValue);
        addArgs(newFrame);

        while (stack.getTop() != targetFrame) {
            Frame& frame = *stack.getTop();

            SchedulerMessage message = frame.getFunction().invoke(vm, frame, task);
            switch (message.type) {
                case SchedulerMessageType::Errored: return {}; // TODO: do something proper
                case SchedulerMessageType::Called: {
                    Frame& newFrame2 = stack.pushFrame(*message.call.function, &(*stack.getTop())[message.call.returnRegister]);

                    for (uint16_t i = 0; i < message.call.function->getParameterCount(); i++) {
                        newFrame2[i] = frame[message.call.argsBegin + i];
                    }

                    vm.memoryManager().safepoint(vm);

                    break;
                }
                case SchedulerMessageType::Returned: {
                    Value* returnRegister = stack.getTop()->returnRegister();
                    stack.popFrame();
                    *returnRegister = message.returnValue; // returnRegister should never be nullptr as long as dumbasses don't touch it
                    vm.memoryManager().safepoint(vm);
                    break;
                }
                case SchedulerMessageType::Yielded:
                case SchedulerMessageType::Awaiting:
                    break; // genuinely just ignore any attempts at async code when in the native environment
            }
        }

        return returnValue;
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
        if (module == nullptr) return nullptr;
        return reinterpret_cast<VMFunction>(module->getFunction(name));
    }

    VMValue CallFunctionV(BibbleVM* vm_, BibbleInterface* interface, VMFunction function_, va_list args);

    VMValue CallFunction(BibbleVM* vm_, BibbleInterface* interface, VMFunction function_, ...) {
        va_list args;
        va_start(args, function_);
        VMValue result = CallFunctionV(vm_, interface, function_, args);
        va_end(args);
        return result;
    }

    VMValue CallFunctionV(BibbleVM* vm_, BibbleInterface* interface, VMFunction function_, va_list args) {
        VM& vm = GetVM(vm_);
        executor::Function* function = GetFunction(function_);
        if (function == nullptr) return {};
        executor::Task* task = ExtractTask(interface);

        Value returnValue = ExecuteFunction(vm, function, task, [&args, function](executor::Frame& frame) {
            for (uint16_t i = 0; i < function->getParameterCount(); i++) {
                VMArgument arg = va_arg(args, VMArgument);
                frame[i] = Value::FromNative(arg.value, arg.reference);
            }
        });

        return returnValue.toNative(vm.memoryManager());
    }

    VMValue CallFunctionA(BibbleVM* vm_, BibbleInterface* interface, VMFunction function_, const VMArgument* args) {
        VM& vm = GetVM(vm_);
        executor::Function* function = GetFunction(function_);
        if (function == nullptr) return {};
        executor::Task* task = ExtractTask(interface);

        Value returnValue = ExecuteFunction(vm, function, task, [args, function](executor::Frame& frame) {
            for (uint16_t i = 0; i < function->getParameterCount(); i++) {
                const VMArgument& arg = args[i];
                frame[i] = Value::FromNative(arg.value, arg.reference);
            }
        });

        return returnValue.toNative(vm.memoryManager());
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
        interface->GetFunction = GetFunction;
        interface->CallFunction = CallFunction;
        interface->CallFunctionV = CallFunctionV;
        interface->CallFunctionA = CallFunctionA;
    }
}
