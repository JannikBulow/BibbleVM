// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/core/error.h"

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

    class InitImpl {
    private:
        static constexpr IntrinsicFunctionImpl init = +[](VM& vm, executor::Frame& frame, executor::Task* task) -> executor::SchedulerMessage {
            if (!vm.pluginManager().load("/home/jannik/CLionProjects/bibble-stdlib/tests/libbibble_stdlib.so")) {
                return executor::SchedulerMessage::Errored(Error::RUNTIME_ERROR, vm.memoryManager().allocateString(vm, "libbibble_stdlib.so failed to load")->asString());
            }
            return executor::SchedulerMessage::Returned(Value());
        };

        static constexpr frozen::unordered_map<frozen::string, IntrinsicFunctionImpl, 1> functions = {
            {"init", init},
        };

    public:
        static const IntrinsicFunctionImpl* LookupFunction(std::string_view name) {
            auto it = functions.find(name);
            if (it == functions.end()) return nullptr;
            return &it->second;
        }

        static constexpr IntrinsicModuleImpl Module = {LookupFunction};
    };

    class RuntimeImpl {
    private:
        static constexpr IntrinsicFunctionImpl get_property = +[](VM& vm, executor::Frame& frame, executor::Task* task) -> executor::SchedulerMessage {
            oop::Object* key = frame[0].obj;
            if (key->kind != oop::ObjectKind::String) return executor::SchedulerMessage::Errored(Error::INVALID_OBJECT_KIND);

            String value = vm.getProperty(String(key->asString()));
            return executor::SchedulerMessage::Returned(value);
        };

        static constexpr IntrinsicFunctionImpl set_property = +[](VM& vm, executor::Frame& frame, executor::Task* task) -> executor::SchedulerMessage {
            oop::Object* key = frame[0].obj;
            oop::Object* value = frame[1].obj;
            if (key->kind != oop::ObjectKind::String) return executor::SchedulerMessage::Errored(Error::INVALID_OBJECT_KIND);
            if (value->kind != oop::ObjectKind::String) return executor::SchedulerMessage::Errored(Error::INVALID_OBJECT_KIND);

            vm.setProperty(key->asString(), value->asString());

            return executor::SchedulerMessage::Returned(Value());
        };

        static constexpr IntrinsicFunctionImpl exit = +[](VM& vm, executor::Frame& frame, executor::Task* task) -> executor::SchedulerMessage {
            Int status = frame[0].i;
            vm.exit([status] {
                std::exit(status);
            });
        };

        static constexpr frozen::unordered_map<frozen::string, IntrinsicFunctionImpl, 3> functions = {
            {"get_property", get_property},
            {"set_property", set_property},
            {"exit", exit},
        };

    public:
        static const IntrinsicFunctionImpl* LookupFunction(std::string_view name) {
            auto it = functions.find(name);
            if (it == functions.end()) return nullptr;
            return &it->second;
        }

        static constexpr IntrinsicModuleImpl Module = {LookupFunction};
    };

    constexpr frozen::unordered_map<frozen::string, const IntrinsicModuleImpl*, 2> modules = {
        {"bibble.init", &InitImpl::Module},
        {"bibble.runtime", &RuntimeImpl::Module}
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
