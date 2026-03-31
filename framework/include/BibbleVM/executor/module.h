// Copyright 2026 Jannik Laugmand Bülow

#ifndef BIBBLEVM_EXECUTOR_MODULE_H
#define BIBBLEVM_EXECUTOR_MODULE_H 1

#include "BibbleVM/executor/function.h"

#include "BibbleVM/util/string_pool.h"

namespace bibblevm::executor {
    class BIBBLEVM_EXPORT Module {
    public:
        Module() = default;
        Module(const String& name, ConstPool constPool, uint16_t functionCount, Function* functions);

        String getName() const { return mName; }
        const ConstPool& getConstPool() const { return mConstPool; }

        Function* getFunction(String name) const;
        Function* getFunction(std::string_view name) const;

    private:
        String mName;

        ConstPool mConstPool;

        uint16_t mFunctionCount;
        Function* mFunctions;
    };
}

#endif //BIBBLEVM_EXECUTOR_MODULE_H