// Copyright 2026 Jannik Laugmand Bülow

#include <BibbleVM/util/string_pool.h>
#include <BibbleVM/init.h>

#include <iostream>

int main(int argc, char** argv) {
    bibblevm::InitDependencies();

    bibblevm::StringPool pool;

    bibblevm::String s1 = pool.intern("Hello World");

    std::cout << s1 << "\n";

    return 0;
}