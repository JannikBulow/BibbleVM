// Copyright 2026 Jannik Laugmand Bülow

#include <BibbleVM/linker/linker.h>

#include <BibbleVM/init.h>
#include <BibbleVM/vm.h>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "no input file" << std::endl;
        return 1;
    }

    bibblevm::InitDependencies();

    bibblevm::Config config;
    config.debug.enableDebugLogging = true;

    bibblevm::VM vm(config);

    if (!vm.pluginManager().load("libBibbleVM-testplugin")) return 1;

    std::unique_ptr<bibblevm::linker::Module> module = std::make_unique<bibblevm::linker::Module>();
    if (!bibblevm::linker::LoadModule(vm, *module, argv[1])) return 1;
    module->setStage(bibblevm::linker::Stage::Ready);
    vm.addModule(std::move(module));

    bibblevm::linker::Module* mainModule = vm.getModule("Main");
    if (mainModule == nullptr) return 2;

    bibblevm::executor::Function* mainFunction = mainModule->linkedModule().getFunction("main");
    if (mainFunction == nullptr) return 3;

    bibblevm::executor::Task* task = vm.scheduler().schedule(vm, *mainFunction, bibblevm::executor::MapPriorityToLevel(vm, bibblevm::executor::TaskPriority::Critical), nullptr);

    bibblevm::oop::Object** mainFutureRef = vm.memoryManager().newGlobalStrongReference(task->completionFuture->asObject());

    vm.scheduler().run(vm);

    if (!(*mainFutureRef)->asFuture()->ready) return 4;
    return (*mainFutureRef)->asFuture()->value.l;
}