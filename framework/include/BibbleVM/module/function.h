// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_FUNCTION_H
#define BIBBLEVM_FUNCTION_H 1

#include "BibbleVM/executor/instruction.h"

#include "BibbleVM/util/string_pool.h"

#include "BibbleVM/api.h"

namespace bibblevm {
    enum class FunctionFlags : uint16_t {
        Async = 0x0001,
        Native = 0x0002,
    };

    class BIBBLEVM_EXPORT Function {
    public:
        Function(const String& name, FunctionFlags flags, InstructionList instructions)
            : mName(name), mFlags(flags), mInstructions(std::move(instructions)) {}

        String getName() const { return mName; }
        FunctionFlags getFlags() const { return mFlags; }
        const InstructionList& getInstructions() const { return mInstructions; }

    private:
        String mName;
        FunctionFlags mFlags;
        InstructionList mInstructions;
    };

    class BIBBLEVM_EXPORT FunctionList {
    public:
        struct Iterator {
            Function* value;

            Iterator(Function* value) : value(value) {}

            Iterator& operator++() {
                value++;
                return *this;
            }

            Iterator operator++(int) {
                Iterator tmp = *this;
                ++value;
                return tmp;
            }

            Function& operator*() const {
                return *value;
            }

            bool operator==(const Iterator& other) const {
                return value == other.value;
            }

            bool operator!=(const Iterator& other) const {
                return value != other.value;
            }
        };

        FunctionList(std::unique_ptr<Function[]> functions, size_t count)
            : mFunctions(std::move(functions))
            , mCount(count) {}

        Function* get() const {
            return mFunctions.get();
        }

        size_t getCount() const {
            return mCount;
        }

        Function* getByName(std::string_view name) const {
            for (auto& function : *this) {
                if (function.getName() == name) {
                    return &function;
                }
            }
            return nullptr;
        }

        Function* getByName(String name) const {
            for (auto& function : *this) {
                if (function.getName() == name) {
                    return &function;
                }
            }
            return nullptr;
        }

        Function& operator[](size_t index) const {
            return mFunctions[index];
        }

        Iterator begin() const { return {mFunctions.get()}; }
        Iterator end() const { return {nullptr}; }

    private:
        std::unique_ptr<Function[]> mFunctions;
        size_t mCount;
    };
}

#endif // BIBBLEVM_FUNCTION_H
