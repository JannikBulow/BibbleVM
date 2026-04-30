// Copyright 2026 Jannik Laugmand Bülow

#include "BibbleVM/core/module/instruction.h"

#include "BibbleVM/core/opcodes.h"

#include "BibbleVM/linker/intrinsics.h"
#include "BibbleVM/linker/linker.h"

#include "BibbleVM/native/trampoline.h"

#include "BibbleVM/vm.h"

#include <unordered_map>

namespace bibblevm::linker {
    struct ArgReader {
        module::BytecodeStream current;
        const module::BytecodeStream end;

        ArgReader(module::BytecodeStream current, module::BytecodeStream end)
            : current(current)
            , end(end) {}

        bool has(size_t length) const {
            return static_cast<size_t>(end - current) >= length;
        }

        template<class T>
        std::optional<uint64_t> readLE() {
            if (!has(sizeof(T))) return std::nullopt;

            uint64_t value = 0;
            for (size_t i = 0; i < sizeof(T); i++) {
                value |= static_cast<uint64_t>(current[i]) << (8 * i);
            }

            current += sizeof(T);
            return value;
        }

        std::optional<uint64_t> readRegister(bool wide) {
            return wide ? readLE<uint16_t>() : readLE<uint8_t>();
        }

        std::optional<uint64_t> readConstPoolIndex(bool wide) {
            return wide ? readLE<uint16_t>() : readLE<uint8_t>();
        }

        std::optional<uint64_t> readImmediate(bool wide, bool huge = false, bool gigantic = false) {
            if (gigantic) return readLE<uint64_t>();
            if (huge) return readLE<uint32_t>();
            if (wide) return readLE<uint16_t>();
            return readLE<uint8_t>();
        }
    };

    executor::ConstPool LinkConstPool(VM& vm, GrowingArenaAllocator& arena, const module::ConstPool& constPool, const module::Module& module, oop::Class* classes, executor::Function* functions) {
        uint16_t entryCount = constPool.getEntryCount();
        module::ConstPool::Entry* entries = constPool.getEntries();
        auto* linkedEntries = arena.allocate<Value>(entryCount);

        for (uint16_t i = 1; i < entryCount; i++) {
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
                case module::ConstPool::CLASS_INFO: {
                    module::ConstPool::ClassInfo ci = entry.u.ci;
                    
                    break;
                }
                case module::ConstPool::FIELD_INFO:
                    break;
                case module::ConstPool::METHOD_INFO:
                    break;
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

    bool LinkClasses(oop::Class* classes, VM& vm, GrowingArenaAllocator& arena, const executor::ConstPool& moduleConstPool, const module::Module& module) {
        for (uint16_t i = 0; i < module.classCount; i++) {
            const module::Class& clas = module.classes[i];
            oop::Class& linkedClass = classes[i];

            oop::Class* superClass = clas.superClass == 0 ? nullptr : moduleConstPool.get(clas.superClass).ci;

            oop::Field* fields = arena.allocate<oop::Field>(clas.fieldCount);
            for (uint16_t j = 0; j < clas.fieldCount; j++) {
                module::Field& field = clas.fields[j];
                fields[j] = oop::Field(static_cast<oop::Type>(field.typeID), moduleConstPool.get(field.name).obj->asString());
            }

            oop::Method* methods = arena.allocate<oop::Method>(clas.methodCount);
            for (uint16_t j = 0; j < clas.methodCount; j++) {
                module::Method& method = clas.methods[j];
                methods[j] = oop::Method(moduleConstPool.get(method.name).obj->asString(), moduleConstPool.get(method.function).fni);
            }

            linkedClass = oop::Class(linkedClass.getName(), superClass, {fields, clas.fieldCount}, {methods, clas.methodCount}, arena);
        }

        return true;
    }

    std::optional<executor::InstructionArguments> DecodeArgs(module::Instruction instruction, ArgReader& argReader) {
        executor::InstructionArguments args{};

        opcodeutils::PrefixState p = instruction.prefixState;

#define OPT_ASSIGN_OR_RETURN(stmt, var) if (auto opt_ = stmt; opt_.has_value()) var = opt_.value(); else return std::nullopt
        switch (static_cast<Opcodes>(instruction.opcode)) {
            case NOP:
                break;
            case MOV:
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand0), args.a);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand1), args.b);
                break;
            case MOV_RANGE:
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand0), args.a);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand1), args.b);
                OPT_ASSIGN_OR_RETURN(argReader.readImmediate(p.wideOperand2), args.c);
                break;
            case SWAP:
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand0), args.a);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand1), args.b);
                break;
            case LOAD_CONST:
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand0), args.a);
                OPT_ASSIGN_OR_RETURN(argReader.readConstPoolIndex(p.wideOperand1), args.b);
                break;
            case LOAD_IMM:
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand0), args.a);
                OPT_ASSIGN_OR_RETURN(argReader.readImmediate(p.wideOperand1, p.hugeImmediate, p.giganticImmediate), args.b);
                break;
            case ADD:
            case SUB:
            case MUL:
            case SDIV:
            case UDIV:
            case SMOD:
            case UMOD:
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand0), args.a);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand1), args.b);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand2), args.c);
                break;
            case NEG:
            case ABS:
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand0), args.a);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand1), args.b);
                break;
            case AND:
            case OR:
            case XOR:
            case NOT:
            case SHL:
            case SHR:
            case SAR:
            case FADD:
            case FSUB:
            case FMUL:
            case FDIV:
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand0), args.a);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand1), args.b);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand2), args.c);
                break;
            case FNEG:
            case FABS:
            case TR8:
            case TR16:
            case TR32:
            case SEX8:
            case SEX16:
            case SEX32:
            case ZEX8:
            case ZEX16:
            case ZEX32:
            case I2F:
            case U2F:
            case I2D:
            case U2D:
            case F2I:
            case F2U:
            case D2I:
            case D2U:
            case F2D:
            case D2F:
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand0), args.a);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand1), args.b);
                break;
            case ICMP:
            case UCMP:
            case FCMP:
            case STRCMP:
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand0), args.a);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand1), args.b);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand2), args.c);
                break;
            case INC:
            case DEC:
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand0), args.a);
                OPT_ASSIGN_OR_RETURN(argReader.readImmediate(p.wideOperand1, p.hugeImmediate, p.giganticImmediate), args.b);
                break;
            case JMP:
                OPT_ASSIGN_OR_RETURN(argReader.readImmediate(p.wideOperand0, p.hugeImmediate, p.giganticImmediate), args.a);
                break;
            case JEQ:
            case JNE:
            case JLT:
            case JLE:
            case JGT:
            case JGE:
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand0), args.a);
                OPT_ASSIGN_OR_RETURN(argReader.readImmediate(p.wideOperand1, p.hugeImmediate, p.giganticImmediate), args.b);
                break;
            case RESERVED_FOR_SWITCH_0:
            case RESERVED_FOR_SWITCH_1:
            case RESERVED_FOR_SWITCH_2:
            case RESERVED_FOR_SWITCH_3:
                break;
            case NEWINSTANCE:
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand0), args.a);
                OPT_ASSIGN_OR_RETURN(argReader.readConstPoolIndex(p.wideOperand1), args.b);
                break;
            case NEWARRAY:
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand0), args.a);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand1), args.b);
                OPT_ASSIGN_OR_RETURN(argReader.readImmediate(false), args.c);
                break;
            case NEWSTRING:
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand0), args.a);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand1), args.b);
                break;
            case NEWFUTURE:
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand0), args.a);
                break;
            case OBJKIND:
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand0), args.a);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand1), args.b);
                break;
            case ISKIND:
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand0), args.a);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand1), args.b);
                OPT_ASSIGN_OR_RETURN(argReader.readImmediate(false), args.c);
                break;
            case INSTANCEOF:
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand0), args.a);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand1), args.b);
                OPT_ASSIGN_OR_RETURN(argReader.readConstPoolIndex(p.wideOperand2), args.c);
                break;
            case GETFIELD:
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand0), args.a);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand1), args.b);
                OPT_ASSIGN_OR_RETURN(argReader.readConstPoolIndex(p.wideOperand2), args.c);
                break;
            case SETFIELD:
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand0), args.a);
                OPT_ASSIGN_OR_RETURN(argReader.readConstPoolIndex(p.wideOperand1), args.b);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand2), args.c);
                break;
            case DISPATCHMETHOD:
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand0), args.a);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand1), args.b);
                OPT_ASSIGN_OR_RETURN(argReader.readConstPoolIndex(p.wideOperand2), args.c);
                break;
            case GETCLASS:
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand0), args.a);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand1), args.b);
                break;
            case ARRAYLENGTH:
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand0), args.a);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand1), args.b);
                break;
            case ARRAYGET:
            case ARRAYSET:
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand0), args.a);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand1), args.b);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand2), args.c);
                break;
            case STRLENGTH:
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand0), args.a);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand1), args.b);
                break;
            case STRGET:
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand0), args.a);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand1), args.b);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand2), args.c);
                break;
            case STR2ARRAY:
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand0), args.a);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand1), args.b);
                break;
            case RESOLVE:
            case CANCEL:
            case ISFUTUREREADY:
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand0), args.a);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand1), args.b);
                break;
            case POLL:
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand0), args.a);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand1), args.b);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand2), args.c);
                break;
            case AWAIT:
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand0), args.a);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand1), args.b);
                break;
            case CALL:
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand0), args.a);
                OPT_ASSIGN_OR_RETURN(argReader.readConstPoolIndex(p.wideOperand1), args.b);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand2), args.c);
                break;
            case TAIL_CALL:
                OPT_ASSIGN_OR_RETURN(argReader.readConstPoolIndex(p.wideOperand0), args.a);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand1), args.b);
                break;
            case CALLA:
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand0), args.a);
                OPT_ASSIGN_OR_RETURN(argReader.readConstPoolIndex(p.wideOperand1), args.b);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand2), args.c);
                break;
            case CALLAP:
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand0), args.a);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand1), args.b);
                OPT_ASSIGN_OR_RETURN(argReader.readConstPoolIndex(p.wideOperand2), args.c);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand3), args.d);
                break;
            case CALLARP:
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand0), args.a);
                OPT_ASSIGN_OR_RETURN(argReader.readImmediate(false), args.b);
                OPT_ASSIGN_OR_RETURN(argReader.readConstPoolIndex(p.wideOperand1), args.c);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand2), args.d);
                break;
            case CALL_DYN:
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand0), args.a);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand1), args.b);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand2), args.c);
                break;
            case TAIL_CALL_DYN:
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand0), args.a);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand1), args.b);
                break;
            case CALLA_DYN:
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand0), args.a);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand1), args.b);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand2), args.c);
                break;
            case CALLAP_DYN:
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand0), args.a);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand1), args.b);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand2), args.c);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand3), args.d);
                break;
            case CALLARP_DYN:
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand0), args.a);
                OPT_ASSIGN_OR_RETURN(argReader.readImmediate(false), args.b);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand1), args.c);
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand2), args.d);
                break;
            case RETURN:
                OPT_ASSIGN_OR_RETURN(argReader.readRegister(p.wideOperand0), args.a);
                break;
            case YIELD:
                break;
        }
#undef OPT_ASSIGN_OR_RETURN

        return args;
    }

    executor::Instruction* DecodeInstructions(VM& vm, GrowingArenaAllocator& arena, const module::Function& function) {
        module::BytecodeStream current = function.bytecode;
        module::BytecodeStream end = function.bytecode + function.bytecodeSize;

        size_t instructionCount = module::DecodeInstructionCount(current, end);

        executor::Instruction* instructions = arena.allocate<executor::Instruction>(instructionCount);

        // For patching branching instructions
        std::vector<module::BytecodeStream> instStart;
        std::vector<module::BytecodeStream> instEnd;
        std::unordered_map<module::BytecodeStream, size_t> byteToIndex;
        instStart.reserve(instructionCount);
        instEnd.reserve(instructionCount);
        byteToIndex.reserve(instructionCount);

        for (size_t i = 0; i < instructionCount; i++) {
            instStart.push_back(current);
            std::optional<module::Instruction> instructionOpt = module::FetchInstruction(current, end);
            if (!instructionOpt.has_value()) return nullptr;
            instEnd.push_back(current);
            byteToIndex[instStart[i]] = i;

            module::Instruction instruction = instructionOpt.value();

            executor::Interpreter interpreter = executor::GetInterpreter(vm, instruction.opcode);
            if (interpreter == nullptr) return nullptr;

            ArgReader argReader(instruction.argsBegin, instruction.argsEnd);
            std::optional<executor::InstructionArguments> decodedArgsOpt = DecodeArgs(instruction, argReader);
            if (!decodedArgsOpt.has_value()) return nullptr;

            instructions[i] = executor::Instruction(instruction.opcode, interpreter, decodedArgsOpt.value());
        }

        // Patch branch instructions
        for (size_t i = 0; i < instructionCount; i++) {
            executor::Instruction& instruction = instructions[i];
            if (!opcodeutils::IsBranch(instruction.opcode)) continue;

            int64_t offset = static_cast<int64_t>((instruction.opcode == JMP) ? instruction.args.a : instruction.args.b);

            module::BytecodeStream target = instEnd[i] + offset;
            auto it = byteToIndex.find(target);
            if (it == byteToIndex.end()) {
                return nullptr;
            }

            size_t targetIndex = it->second;

            if (instruction.opcode == JMP) instruction.args.a = targetIndex - i;
            else instruction.args.b = targetIndex - i;
        }

        return instructions;
    }

    bool LinkFunctions(oop::Class* classes, executor::Function* functions, VM& vm, GrowingArenaAllocator& arena, const executor::ConstPool& constPool, const module::Module& module, executor::Module& linkedModule) {
        String moduleName = constPool.get(module.name).obj->asString();
        const IntrinsicModule* intrinsicModule = GetIntrinsicsModule(moduleName);

        for (uint16_t i = 0; i < module.functionCount; i++) {
            const module::Function& function = module.functions[i];
            executor::Function& linkedFunction = functions[i];

            executor::FunctionKind kind;
            if (function.flags & module::FUNC_NATIVE) {
                kind = executor::FunctionKind::Native;
            } else {
                kind = executor::FunctionKind::Normal;
            }

            executor::Instruction* instructions = nullptr;
            if (kind != executor::FunctionKind::Native) instructions = DecodeInstructions(vm, arena, function);

            linkedFunction = executor::Function(linkedModule, linkedFunction.getName(), kind, function.registerCount, function.parameterCount, instructions);

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

                    void* implementation = vm.pluginManager().getNativeFunction<void*>(moduleName, linkedFunction.getName());

                    if (implementation != nullptr) {
                        linkedFunction.entryPoint() = native::FunctionTrampoline;
                        linkedFunction.implementation() = implementation;
                    } //TODO: else error

                    break;
                }
            }
        }

        return true;
    }

    bool LinkModule(VM& vm, Module& module) {
        if (module.getStage() != Stage::Preverified) return false;

        module::Module& rawModule = module.rawModule();
        executor::Module& linkedModule = module.linkedModule();

        oop::Class* linkedClasses = module.arena().allocate<oop::Class>(rawModule.classCount);
        for (uint16_t i = 0; i < rawModule.classCount; i++) {
            module::Class& clas = rawModule.classes[i];
            String name = vm.stringPool().intern(vm, rawModule.constPool.getEntries()[clas.name - rawModule.constPool.getEntryCount()].u.str);
            linkedClasses[i] = oop::Class(name);
        }

        executor::Function* linkedFunctions = module.arena().allocate<executor::Function>(rawModule.functionCount);
        for (uint16_t i = 0; i < rawModule.functionCount; i++) {
            module::Function& function = rawModule.functions[i];
            String name = vm.stringPool().intern(vm, rawModule.constPool.getEntries()[function.name].u.str);
            linkedFunctions[i] = executor::Function(linkedModule, name); // holy fuck this is ugly
        }

        executor::ConstPool linkedConstPool = LinkConstPool(vm, module.arena(), rawModule.constPool, rawModule, linkedClasses, linkedFunctions);

        if (!LinkClasses(linkedClasses, vm, module.arena(), linkedConstPool, rawModule)) return false;
        if (!LinkFunctions(linkedClasses, linkedFunctions, vm, module.arena(), linkedConstPool, rawModule, linkedModule)) return false;

        linkedModule = executor::Module(linkedConstPool.get(rawModule.name).obj->asString(), std::move(linkedConstPool), rawModule.classCount, linkedClasses, rawModule.functionCount, linkedFunctions);

        module.setStage(Stage::Linked);

        return true;
    }
}
