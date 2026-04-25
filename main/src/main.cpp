// Copyright 2026 Jannik Laugmand Bülow

#include <BibbleVM/core/opcodes.h>

#include <BibbleVM/linker/linker.h>

#include <BibbleVM/init.h>
#include <BibbleVM/vm.h>

static uint8_t mainFunctionCode[] = {
    bibblevm::CALLA, 0, 7, 0,
    bibblevm::LOAD_CONST, 1, 2,
    bibblevm::CALL, 1, 9, 1,
    bibblevm::AWAIT, 1, 0,
    bibblevm::RETURN, 1
};
static constexpr uint32_t mainFunctionCodeLength = sizeof(mainFunctionCode);

void DefineIntrinsicsModule(bibblevm::linker::Module& linkerModule) {
    auto* moduleConstEntries = linkerModule.arena().allocate<bibblevm::module::ConstPool::Entry>(3);
    moduleConstEntries[1].tag = bibblevm::module::ConstPool::Tag::STRING;
    moduleConstEntries[1].u.str = "Intrinsics";
    moduleConstEntries[2].tag = bibblevm::module::ConstPool::Tag::STRING;
    moduleConstEntries[2].u.str = "print";
    bibblevm::module::ConstPool moduleConstPool(3, moduleConstEntries);

    auto* functions = linkerModule.arena().allocate<bibblevm::module::Function>(1);
    functions[0] = {2, bibblevm::module::FUNC_NATIVE, 1, 1, {}, 0, nullptr};

    bibblevm::module::Module module(bibblevm::module::MAGIC, 0, 1, moduleConstPool, 0, 1, nullptr, functions);

    linkerModule.rawModule() = module;
    linkerModule.setStage(bibblevm::linker::Stage::PreVerified);
}

void DefineMainModule(bibblevm::linker::Module& linkerModule) {
    auto* moduleConstEntries = linkerModule.arena().allocate<bibblevm::module::ConstPool::Entry>(10);
    moduleConstEntries[1].tag = bibblevm::module::ConstPool::STRING;
    moduleConstEntries[1].u.str = "Main";
    moduleConstEntries[2].tag = bibblevm::module::ConstPool::STRING;
    moduleConstEntries[2].u.str = "main";
    moduleConstEntries[3].tag = bibblevm::module::ConstPool::STRING;
    moduleConstEntries[3].u.str = "test";
    moduleConstEntries[4].tag = bibblevm::module::ConstPool::STRING;
    moduleConstEntries[4].u.str = "Intrinsics";
    moduleConstEntries[5].tag = bibblevm::module::ConstPool::STRING;
    moduleConstEntries[5].u.str = "print";
    moduleConstEntries[6].tag = bibblevm::module::ConstPool::MODULE_INFO;
    moduleConstEntries[6].u.mi = {1};
    moduleConstEntries[7].tag = bibblevm::module::ConstPool::FUNCTION_INFO;
    moduleConstEntries[7].u.fni = {6, 3};
    moduleConstEntries[8].tag = bibblevm::module::ConstPool::MODULE_INFO;
    moduleConstEntries[8].u.mi = {4};
    moduleConstEntries[9].tag = bibblevm::module::ConstPool::FUNCTION_INFO;
    moduleConstEntries[9].u.fni = {8, 5};
    bibblevm::module::ConstPool moduleConstPool(10, moduleConstEntries);

    auto* functions = linkerModule.arena().allocate<bibblevm::module::Function>(2);
    functions[0] = {2, 0, 4, 0, {}, mainFunctionCodeLength, mainFunctionCode};
    functions[1] = {3, bibblevm::module::FUNC_NATIVE, 0, 0, {}, 0, nullptr};

    bibblevm::module::Module module(bibblevm::module::MAGIC, 0, 1, moduleConstPool, 0, 2, nullptr, functions);

    linkerModule.rawModule() = module;
    linkerModule.setStage(bibblevm::linker::Stage::PreVerified);
}

int main(int argc, char** argv) {
    bibblevm::InitDependencies();

    bibblevm::Config config;
    config.debug.enableDebugLogging = true;

    bibblevm::VM vm(config);

    if (!vm.pluginManager().load("libBibbleVM-testplugin")) return 1;

    std::unique_ptr<bibblevm::linker::Module> module = std::make_unique<bibblevm::linker::Module>();
    DefineIntrinsicsModule(*module);
    if (!bibblevm::linker::LinkModule(vm, *module)) return 1;
    module->setStage(bibblevm::linker::Stage::Ready);
    vm.addModule(std::move(module));

    module = std::make_unique<bibblevm::linker::Module>();
    DefineMainModule(*module);
    if (!bibblevm::linker::LinkModule(vm, *module)) return 1;
    module->setStage(bibblevm::linker::Stage::Ready);
    vm.addModule(std::move(module));

    bibblevm::linker::Module* mainModule = vm.getModule("Main");
    if (mainModule == nullptr) return 2;

    bibblevm::executor::Function* mainFunction = mainModule->linkedModule().getFunction("main");
    if (mainFunction == nullptr) return 3;

    bibblevm::executor::Task* task = vm.scheduler().schedule(vm, *mainFunction, 0, nullptr);

    vm.scheduler().run(vm);

    if (!task->completionFuture->ready) return 4;

    return task->completionFuture->value.l;
}