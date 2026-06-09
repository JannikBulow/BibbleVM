// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/linker/intrinsics.h"

#include "BibbleVM/vm.h"

#include <libos/file.h>

#include <frozen/string.h>
#include <frozen/unordered_map.h>

namespace bibblevm::linker {
    struct IntrinsicFunctionImpl {
        IntrinsicFunction publicView;

        constexpr IntrinsicFunctionImpl(executor::EntryPoint entryPoint) : publicView(entryPoint) {}
    };

    struct IntrinsicModuleImpl {
        const IntrinsicFunctionImpl* (*lookup)(std::string_view);
    };

    class IntrinsicsImpl {
    private:
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

        static constexpr IntrinsicFunctionImpl print = +[](VM& vm, executor::Frame& frame, executor::Task* task) -> executor::SchedulerMessage {
            return printHelper(vm, std::to_string(frame[0].l));
        };

        static constexpr IntrinsicFunctionImpl printString = +[](VM& vm, executor::Frame& frame, executor::Task* task) -> executor::SchedulerMessage {
            oop::Object* object = frame[0].obj;

            if (object == nullptr) return executor::SchedulerMessage::Errored(Error::NULL_REFERENCE);
            if (object->kind != oop::ObjectKind::String) return executor::SchedulerMessage::Errored(Error::INVALID_OBJECT_KIND);

            oop::StringObject* string = object->asString();

            return printHelper(vm, std::string_view(string->bytes, string->lengthBytes));
        };

        static constexpr frozen::unordered_map<frozen::string, IntrinsicFunctionImpl, 2> functions = {
            {"print", print},
            {"printString", printString},
        };

    public:
        static const IntrinsicFunctionImpl* LookupFunction(std::string_view name) {
            auto it = functions.find(name);
            if (it == functions.end()) return nullptr;
            return &it->second;
        }

        static constexpr IntrinsicModuleImpl Module = {LookupFunction};
    };

    constexpr frozen::unordered_map<frozen::string, const IntrinsicModuleImpl*, 1> modules = {
        {"Intrinsics", &IntrinsicsImpl::Module}
    };

    const IntrinsicModule* GetIntrinsicsModule(std::string_view name) {
        auto it = modules.find(name);
        if (it == modules.end()) return nullptr;
        return reinterpret_cast<const IntrinsicModule*>(it->second);
    }

    const IntrinsicFunction* GetIntrinsicsFunction(const IntrinsicModule* module, std::string_view name) {
        if (module == nullptr) return nullptr;
        const IntrinsicModuleImpl* moduleImpl = reinterpret_cast<const IntrinsicModuleImpl*>(module);
        return &moduleImpl->lookup(name)->publicView;
    }
}
