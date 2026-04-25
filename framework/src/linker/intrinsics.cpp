// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/linker/intrinsics.h"

#include <iostream>

namespace bibblevm::linker {
    namespace Intrinsics {
        static executor::SchedulerMessage print(VM& vm, executor::Frame& frame, executor::Task* task) {
            std::cout << frame[0].l << std::endl;
            return executor::SchedulerMessage::Returned(Value(0));
        }
    }

    static constexpr IntrinsicFunction Intrinsics_functions[] = {
        {"print", Intrinsics::print},
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
        for (size_t i = 0; i < module->functionCount; i++) {
            const IntrinsicFunction& function = module->functions[i];
            if (function.name == name) return &function;
        }
        return nullptr;
    }
}
