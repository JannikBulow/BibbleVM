// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/core/module/instruction.h"

#include "BibbleVM/core/opcodes.h"

#include "BibbleVM/linker/intrinsics.h"
#include "BibbleVM/linker/linker.h"

#include "BibbleVM/vm.h"

namespace bibblevm::linker {
    executor::ConstPool LinkConstPool(VM& vm, GrowingArenaAllocator& arena, const module::ConstPool& constPool, const module::Module& module, executor::Function* functions) {
        uint16_t entryCount = constPool.getEntryCount();
        module::ConstPool::Entry* entries = constPool.getEntries();
        auto* linkedEntries = arena.allocate<Value>(entryCount);

        for (uint16_t i = 0; i < entryCount; i++) {
            auto& entry = entries[i];
            auto& linkedEntry = linkedEntries[i];

            switch (entry.tag) {
                case module::ConstPool::BYTE:
                    linkedEntry.b = entry.u.b;
                    break;
                case module::ConstPool::SHORT:
                    linkedEntry.s = entry.u.s;
                    break;
                case module::ConstPool::INT:
                    linkedEntry.i = entry.u.i;
                    break;
                case module::ConstPool::LONG:
                    linkedEntry.l = entry.u.l;
                    break;
                case module::ConstPool::STRING:
                    linkedEntry.obj = vm.stringPool().intern(vm, entry.u.str).get()->asObject();
                    linkedEntry.isObject = true;
                    break;
                case module::ConstPool::MODULE_INFO: {
                    Module* m = vm.getModule(entries[entry.u.mi.name].u.str); // PreVerifier has ensured this exists, but not that it's fully loaded
                    linkedEntry.mi = &m->linkedModule();
                    break;
                }
                case module::ConstPool::FUNCTION_INFO: {
                    module::ConstPool::FunctionInfo fni = entry.u.fni;
                    if (entries[fni.module].u.mi.name == module.name) { // loading a function from its own module would lead to a permanent stall, so we use a list of partially initialized functions (just names) to link the pointers
                        for (uint16_t j = 0; j < module.functionCount; j++) {
                            if (functions[j].getName() == entries[fni.name].u.str) linkedEntry.fni = &functions[j];
                        }
                    } else {
                        Module* m = vm.getModule(entries[entries[fni.module].u.mi.name].u.str); // PreVerifier has ensured this exists, but not that it's fully loaded
                        m->waitForStage(Stage::Linked);
                        linkedEntry.fni = m->linkedModule().getFunction(entries[entry.u.fi.name].u.str);
                    }
                    break;
                }
            }
        }

        return { entryCount, linkedEntries };
    }

    executor::Instruction* DecodeInstructions(VM& vm, GrowingArenaAllocator& arena, const module::Function& function) {
        module::InstructionStream instructionStream(function.bytecode, function.bytecodeSize);
        size_t instructionCount = instructionStream.decodeInstructionCount();

        auto* instructions = arena.allocate<executor::Instruction>(instructionCount);

        for (size_t i = 0; i < instructionCount; i++) {
            module::Instruction instruction = instructionStream.fetchOne();
            uint8_t a = instruction.getArgumentA();
            uint8_t b = instruction.getArgumentB();
            uint8_t c = instruction.getArgumentC();
            executor::InstructionArguments args{};

            auto ext8to16 = [](uint8_t low, uint8_t high) -> uint16_t { return static_cast<uint16_t>(low) | (static_cast<uint16_t>(high) << 8); };
            auto assignGeneric = [&args, a, b, c] { args.generic = {a, b, c}; };
            auto assignExtAB = [&args, &ext8to16, a, b, c] { args.extAB = {ext8to16(a, b), c}; };
            auto assignExtBC = [&args, &ext8to16, a, b, c] { args.extBC = {a, ext8to16(b, c)}; };

            switch (instruction.getOpcode()) {
                case MOV_HOT_EXT:
                    assignExtAB();
                    break;
                case MOV_EXT_HOT:
                    assignExtBC();
                    break;
                case SWAP_HOT_EXT:
                    assignExtBC();
                    break;
                case LOAD_CONST:
                    assignExtBC();
                    break;
                case LOADB:
                    assignExtAB();
                    break;
                case LOADS:
                    assignExtBC();
                    break;
                case INC:
                case DEC:
                    assignExtBC();
                    break;
                case JMP:
                    args.extJump.branch = static_cast<int32_t>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b) << 8 | static_cast<uint32_t>(c) << 16) / 4; //NOTE: divide by 4 because the interpreter will treat branch as amount of instructions while the bytecode format uses bytes
                    break;
                case JEQ:
                case JNE:
                case JLT:
                case JLE:
                case JGT:
                case JGE:
                    args.extJump.cond = a;
                    args.extJump.branch = static_cast<int32_t>(ext8to16(b, c)) / 4; //NOTE: see JMP on why we divide by 4
                    break;
                case CALL:
                case TAIL_CALL:
                case CALLA:
                case AWAIT:
                    assignExtBC();
                    break;
                case RETURN:
                    assignExtAB();
                    break;

                default:
                    assignGeneric();
                    break;
            }

            instructions[i] = executor::Instruction(executor::GetInterpreter(vm, instruction.getOpcode()), args);
        }

        return instructions;
    }

    bool LinkFunctions(executor::Function* functions, VM& vm, GrowingArenaAllocator& arena, const executor::ConstPool& moduleConstPool, const module::Module& module) {
        const IntrinsicModule* intrinsicModule = GetIntrinsicsModule(String(moduleConstPool.get(module.name).obj->asString()));

        for (uint16_t i = 0; i < module.functionCount; i++) {
            const module::Function& function = module.functions[i];
            executor::Function& linkedFunction = functions[i];

            executor::FunctionKind kind;
            if (function.flags & module::FUNC_NATIVE) {
                kind = executor::FunctionKind::Native;
            } else {
                kind = executor::FunctionKind::Normal;
            }

            executor::ConstPool functionConstPool = LinkConstPool(vm, arena, function.constPool, module, functions);
            std::optional<executor::ConstPool> constPoolOpt = moduleConstPool.merge(functionConstPool, arena);
            if (!constPoolOpt.has_value()) return false;
            executor::ConstPool& constPool = constPoolOpt.value();

            executor::Instruction* instructions = nullptr;
            if (kind != executor::FunctionKind::Native) instructions = DecodeInstructions(vm, arena, function);

            linkedFunction = executor::Function(linkedFunction.getName(), kind, function.registerCount, function.parameterCount, std::move(functionConstPool), std::move(constPool), instructions);

            switch (kind) {
                case executor::FunctionKind::Normal:
                    linkedFunction.entryPoint() = vm.config().scheduler.autoYielding.enabled ? executor::AutoYieldingBytecodeInterpreter : executor::BytecodeInterpreter; // TODO: cache?
                    break;
                case executor::FunctionKind::Native: {
                    if (intrinsicModule != nullptr) {
                        const IntrinsicFunction* intrinsic = GetIntrinsicsFunction(intrinsicModule, linkedFunction.getName());
                        if (intrinsic != nullptr) {
                            linkedFunction.entryPoint() = intrinsic->entryPoint;
                            break;
                        }
                    }

                    //TODO: search loaded libraries for function
                    break;
                }
            }
        }

        return true;
    }

    bool LinkModule(VM& vm, Module& module) {
        if (module.getStage() != Stage::PreVerified) return false;

        module::Module& rawModule = module.rawModule();
        executor::Module& linkedModule = module.linkedModule();

        auto* linkedFunctions = module.arena().allocate<executor::Function>(rawModule.functionCount);
        for (uint16_t i = 0; i < rawModule.functionCount; i++) {
            module::Function& function = rawModule.functions[i];
            String name{};
            if (function.name >= rawModule.constPool.getEntryCount()) {
                name = vm.stringPool().intern(vm, function.constPool.getEntries()[function.name - rawModule.constPool.getEntryCount()].u.str);
            } else {
                name = vm.stringPool().intern(vm, rawModule.constPool.getEntries()[function.name].u.str);
            }
            linkedFunctions[i] = executor::Function(name); // holy fuck this is ugly
        }

        executor::ConstPool linkedConstPool = LinkConstPool(vm, module.arena(), rawModule.constPool, rawModule, linkedFunctions);

        if (!LinkFunctions(linkedFunctions, vm, module.arena(), linkedConstPool, rawModule)) return false;

        linkedModule = executor::Module(linkedConstPool.get(rawModule.name).obj->asString(), std::move(linkedConstPool), rawModule.functionCount, linkedFunctions);

        module.setStage(Stage::Linked);

        return true;
    }
}
