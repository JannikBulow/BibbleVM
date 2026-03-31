// Copyright 2026 Jannik Laugmand Bülow

#include <BibbleVM/core/opcodes.h>

#include <BibbleVM/linker/linker.h>

#include <BibbleVM/init.h>
#include <BibbleVM/vm.h>

static uint8_t mainFunctionCode[] = {
    bibblevm::LOAD_CONST, 1, 6, 0,
    bibblevm::ASYNC_CALL, 1, 0, 0,
    bibblevm::MOV, 3, 0, 0,
    bibblevm::LOAD_CONST, 1, 8, 0,
    bibblevm::LOAD_CONST, 2, 1, 0,
    bibblevm::CALL, 1, 2, 0,
    bibblevm::AWAIT, 3, 0, 0,
    bibblevm::RETURN, 0, 0, 0
};
static constexpr uint32_t mainFunctionCodeLength = sizeof(mainFunctionCode);

static uint8_t testFunctionCode[] = {
    bibblevm::LOAD_CONST, 0, 8, 0,
    bibblevm::LOAD_CONST, 1, 2, 0,
    bibblevm::CALL, 0, 1, 0,
    bibblevm::LOADS, 0, 69, 0,
    bibblevm::RETURN, 0, 0, 0
};
static constexpr uint32_t testFunctionCodeLength = sizeof(testFunctionCode);

void DefineIntrinsicsModule(bibblevm::linker::Module& linkerModule) {
    auto* moduleConstEntries = linkerModule.arena().allocate<bibblevm::module::ConstPool::Entry>(2);
    moduleConstEntries[0].tag = bibblevm::module::ConstPool::Tag::STRING;
    moduleConstEntries[0].u.str = "Intrinsics";
    moduleConstEntries[1].tag = bibblevm::module::ConstPool::Tag::STRING;
    moduleConstEntries[1].u.str = "print";
    bibblevm::module::ConstPool moduleConstPool(2, moduleConstEntries);

    auto* functions = linkerModule.arena().allocate<bibblevm::module::Function>(1);
    functions[0] = {1, bibblevm::module::FUNC_NATIVE, 1, 1, {}, 0, nullptr};

    bibblevm::module::Module module(bibblevm::module::MAGIC, 0, 0, moduleConstPool, 1, functions);

    linkerModule.rawModule() = module;
    linkerModule.setStage(bibblevm::linker::Stage::PreVerified);
}

void DefineMainModule(bibblevm::linker::Module& linkerModule) {
    auto* moduleConstEntries = linkerModule.arena().allocate<bibblevm::module::ConstPool::Entry>(9);
    moduleConstEntries[0].tag = bibblevm::module::ConstPool::STRING;
    moduleConstEntries[0].u.str = "Main";
    moduleConstEntries[1].tag = bibblevm::module::ConstPool::STRING;
    moduleConstEntries[1].u.str = "main";
    moduleConstEntries[2].tag = bibblevm::module::ConstPool::STRING;
    moduleConstEntries[2].u.str = "test";
    moduleConstEntries[3].tag = bibblevm::module::ConstPool::STRING;
    moduleConstEntries[3].u.str = "Intrinsics";
    moduleConstEntries[4].tag = bibblevm::module::ConstPool::STRING;
    moduleConstEntries[4].u.str = "print";
    moduleConstEntries[5].tag = bibblevm::module::ConstPool::MODULE_INFO;
    moduleConstEntries[5].u.mi = {0};
    moduleConstEntries[6].tag = bibblevm::module::ConstPool::FUNCTION_INFO;
    moduleConstEntries[6].u.fi = {5, 2};
    moduleConstEntries[7].tag = bibblevm::module::ConstPool::MODULE_INFO;
    moduleConstEntries[7].u.mi = {3};
    moduleConstEntries[8].tag = bibblevm::module::ConstPool::FUNCTION_INFO;
    moduleConstEntries[8].u.fi = {7, 4};
    bibblevm::module::ConstPool moduleConstPool(9, moduleConstEntries);

    auto* functions = linkerModule.arena().allocate<bibblevm::module::Function>(2);
    functions[0] = {1, 0, 4, 0, {}, mainFunctionCodeLength, mainFunctionCode};
    functions[1] = {2, 0, 2, 0, {}, testFunctionCodeLength, testFunctionCode};

    bibblevm::module::Module module(bibblevm::module::MAGIC, 0, 0, moduleConstPool, 2, functions);

    linkerModule.rawModule() = module;
    linkerModule.setStage(bibblevm::linker::Stage::PreVerified);
}

int main(int argc, char** argv) {
    bibblevm::InitDependencies();

    bibblevm::VM vm;

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

    bibblevm::executor::Task* task = vm.scheduler().schedule(*mainFunction);
    bibblevm::executor::Future* future = task->completionFuture;
    vm.scheduler().run(vm);

    if (!future->ready) return 4;

    return future->value.i;
}