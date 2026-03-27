// Copyright 2026 Jannik Laugmand Bülow

#include <BibbleVM/init.h>
#include <BibbleVM/vm.h>

#include <iostream>

bool TestInsn1(bibblevm::VM& vm, uint8_t a, uint8_t b, uint8_t c) {
    std::cout << static_cast<int>(a) << " " << static_cast<int>(b) << " " << static_cast<int>(c) << std::endl;
    return true;
}

int main(int argc, char** argv) {
    bibblevm::InitDependencies();

    bibblevm::StringPool stringPool;

    bibblevm::InstructionList mainInstructions {
        bibblevm::Instruction(TestInsn1, 0, 0, 69)
    };

    bibblevm::FunctionList functions {
        bibblevm::Function(stringPool.intern("main"), 0, std::move(mainInstructions))
    };

    bibblevm::VM vm;
    vm.addModule(bibblevm::CreateModulePtr(stringPool.intern("Main"), std::move(functions)));

    bibblevm::Module* entryModule = vm.getEntryPointModule();
    bibblevm::Function* entry = vm.getEntryPointFunction(entryModule);
    if (entry == nullptr) return 1;

    entry->entryPoint(vm);

    return 0;
}