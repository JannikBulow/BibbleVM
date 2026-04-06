// Copyright 2026 Jannik Laugmand Bülow

#include <gtest/gtest.h>

#include <BibbleVM/init.h>

int main(int argc, char** argv) {
    bibblevm::InitDependencies();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
