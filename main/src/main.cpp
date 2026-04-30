// Copyright 2026 Jannik Laugmand Bülow

#include <BibbleVM/core/opcodes.h>

#include <BibbleVM/linker/linker.h>

#include <BibbleVM/init.h>
#include <BibbleVM/vm.h>

#include "../../cmake-build-release-mingw/_deps/bibblebytecode-src/include/BibbleBytecode/module/const_pool.h"

static uint8_t intrinsicsModule[] = {
    0x67, 0x67, 0xAC, 0xFA, // magic
    0, 0, // format version
    1, 0, // name


    // const pool
    3, 0, // entry count

    // #1
    bibblevm::module::ConstPool::Tag::STRING, // tag
    10, 0, 0, 0, // string length
    'I', 'n', 't', 'r', 'i', 'n', 's', 'i', 'c', 's', // string data

    // #2
    bibblevm::module::ConstPool::Tag::STRING, // tag
    5, 0, 0, 0, // string length
    'p', 'r', 'i', 'n', 't', // string data


    0, 0, // class count
    1, 0, // function count


    // classes

    // functions
    // print
    2, 0, // name
    1, 0, // flags - native
    1, 0, // register count
    1, 0, // parameter count
    0, 0, 0, 0 // bytecode size - 0 because native
};
static constexpr size_t intrinsicsModuleLength = sizeof(intrinsicsModule);

static uint8_t mainModule[] = {
    0x67, 0x67, 0xAC, 0xFA, // magic
    0, 0, // format version
    1, 0, // name


    // const pool
    10, 0, // entry count

    // #1
    bibblevm::module::ConstPool::Tag::STRING, // tag
    4, 0, 0, 0, // string length
    'M', 'a', 'i', 'n', // string data

    // #2
    bibblevm::module::ConstPool::Tag::STRING, // tag
    4, 0, 0, 0, // string length
    'm', 'a', 'i', 'n', // string data

    // #3
    bibblevm::module::ConstPool::Tag::STRING, // tag
    4, 0, 0, 0, // string length
    't', 'e', 's', 't', // string data

    // #4
    bibblevm::module::ConstPool::Tag::STRING, // tag
    10, 0, 0, 0, // string length
    'I', 'n', 't', 'r', 'i', 'n', 's', 'i', 'c', 's', // string data

    // #5
    bibblevm::module::ConstPool::Tag::STRING, // tag
    5, 0, 0, 0, // string length
    'p', 'r', 'i', 'n', 't', // string data

    // #6
    bibblevm::module::ConstPool::Tag::MODULE_INFO, // tag
    1, 0, // module name

    // #7
    bibblevm::module::ConstPool::Tag::FUNCTION_INFO, // tag
    6, 0, // module
    3, 0, // function name

    // #8
    bibblevm::module::ConstPool::Tag::MODULE_INFO, // tag
    4, 0, // module name

    // #9
    bibblevm::module::ConstPool::Tag::FUNCTION_INFO, // tag
    8, 0, // module
    5, 0, // function name


    0, 0, // class count
    2, 0, // function count


    // classes

    // functions
    // main
    2, 0, // name
    0, 0, // flags
    4, 0, // register count
    0, 0, // parameter count
    16, 0, 0, 0, // bytecode size
    // bytecode
    bibblevm::CALLA, 0, 7, 0,
    bibblevm::LOAD_CONST, 1, 2,
    bibblevm::CALL, 1, 9, 1,
    bibblevm::AWAIT, 1, 0,
    bibblevm::RETURN, 1,

    // test
    3, 0, // name
    1, 0, // flags - native
    0, 0, // register count
    0, 0, // parameter count
    0, 0, 0, 0 // bytecode size - 0 because native
};
static constexpr size_t mainModuleLength = sizeof(mainModule);

int main(int argc, char** argv) {
    bibblevm::InitDependencies();

    bibblevm::Config config;
    config.debug.enableDebugLogging = true;

    bibblevm::VM vm(config);

    if (!vm.pluginManager().load("libBibbleVM-testplugin")) return 1;

    std::unique_ptr<bibblevm::linker::Module> module = std::make_unique<bibblevm::linker::Module>();
    if (!bibblevm::linker::ReadModuleFromMemory(vm, *module, {intrinsicsModule, intrinsicsModuleLength})) return 1;
    if (!bibblevm::linker::LoadModule(vm, *module, nullptr)) return 1;
    module->setStage(bibblevm::linker::Stage::Ready);
    vm.addModule(std::move(module));

    module = std::make_unique<bibblevm::linker::Module>();
    if (!bibblevm::linker::ReadModuleFromMemory(vm, *module, {mainModule, mainModuleLength})) return 1;
    if (!bibblevm::linker::LoadModule(vm, *module, nullptr)) return 1;
    module->setStage(bibblevm::linker::Stage::Ready);
    vm.addModule(std::move(module));

    bibblevm::linker::Module* mainModule = vm.getModule("Main");
    if (mainModule == nullptr) return 2;

    bibblevm::executor::Function* mainFunction = mainModule->linkedModule().getFunction("main");
    if (mainFunction == nullptr) return 3;

    bibblevm::executor::Task* task = vm.scheduler().schedule(vm, *mainFunction, 0, nullptr);

    bibblevm::oop::Object** mainFutureRef = vm.memoryManager().newGlobalStrongReference(task->completionFuture->asObject());

    vm.scheduler().run(vm);

    if (!(*mainFutureRef)->asFuture()->ready) return 4;
    return (*mainFutureRef)->asFuture()->value.l;
}