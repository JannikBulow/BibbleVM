// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/compiler/aot_trap.h"
#include "BibbleVM/compiler/compiler.h"
#include "BibbleVM/compiler/trampoline.h"

#include "BibbleVM/vm.h"

namespace bibblevm::compiler {
    executor::SchedulerMessage AOTCompileTrap(VM& vm, executor::Frame& frame, executor::Task* task) {
        executor::Function& function = frame.getFunction();

        CompilationUnit* unit = new CompilationUnit();
        function.setCompilationUnit(unit);

        unit->state = CompilationState::Compiling;

        CompileOptions options;
        options.opt = OptLevel::O0;

        compiler::Compiler compiler;
        Code* baseline = compiler.compile(vm, &function, options);

        if (baseline == nullptr) {
            // better to default to interpreter than to take the whole program down with us
            unit->state = CompilationState::Failed;
            function.setEntryPoint(vm.config().scheduler.autoYielding.enabled ? executor::AutoYieldingBytecodeInterpreter : executor::BytecodeInterpreter);
            return function.getEntryPoint()(vm, frame, task);
        }

        unit->baseline = baseline;
        unit->state = CompilationState::Compiled;

        function.setEntryPoint(Trampoline);
        return function.getEntryPoint()(vm, frame, task);
    }
}
