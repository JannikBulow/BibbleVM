// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/linker/intrinsics.h"

#include <libos/file.h>

#include <iostream>

#include "BibbleVM/vm.h"

namespace bibblevm::linker {
    namespace Intrinsics {
        static executor::SchedulerMessage printHelper(VM& vm, std::string_view value) {
            os_file* out;
            os_result res = os_file_get_stdfile(&out, OS_STDOUT);
            if (res != OS_OK) {
                return executor::SchedulerMessage::Errored(Error::USERLAND, vm.describeOSResult(res));
            }

            os_size written = 0;
            res = os_file_write(out, value.data(), value.length(), &written);
            if (res != OS_OK) {
                return executor::SchedulerMessage::Errored(Error::USERLAND, vm.describeOSResult(res));
            }

            res = os_file_write(out, "\n", 1, nullptr);
            if (res != OS_OK) {
                return executor::SchedulerMessage::Errored(Error::USERLAND, vm.describeOSResult(res));
            }

            Value returnValue;
            returnValue.isObject = false;
            returnValue.ul = written;

            return executor::SchedulerMessage::Returned(returnValue);
        }

        static executor::SchedulerMessage print(VM& vm, executor::Frame& frame, executor::Task* task) {
            return printHelper(vm, std::to_string(frame[0].l));
        }

        static executor::SchedulerMessage printString(VM& vm, executor::Frame& frame, executor::Task* task) {
            oop::Object* object = frame[0].obj;

            if (object == nullptr) return executor::SchedulerMessage::Errored(Error::NULL_REFERENCE);
            if (object->kind != oop::ObjectKind::String) return executor::SchedulerMessage::Errored(Error::INVALID_OBJECT_KIND);

            oop::StringObject* string = object->asString();

            return printHelper(vm, std::string_view(string->bytes, string->lengthBytes));
        }
    }

    static constexpr IntrinsicFunction Intrinsics_functions[] = {
        {"print", Intrinsics::print},
        {"printString", Intrinsics::printString},
    };
    static constexpr IntrinsicModule intrinsicModules[] = {
        {"Intrinsics", Intrinsics_functions, std::size(Intrinsics_functions)},
    };

    const IntrinsicModule* GetIntrinsicsModule(std::string_view name) {
        for (const auto& intrinsicModule : intrinsicModules) {
            if (intrinsicModule.name == name) return &intrinsicModule;
        }
        return nullptr;
    }

    const IntrinsicFunction* GetIntrinsicsFunction(const IntrinsicModule* module, std::string_view name) {
        if (module == nullptr) return nullptr;

        for (size_t i = 0; i < module->functionCount; i++) {
            const IntrinsicFunction& function = module->functions[i];
            if (function.name == name) return &function;
        }
        return nullptr;
    }
}
