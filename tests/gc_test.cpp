// Copyright 2026 Jannik Laugmand Bülow

#include <gtest/gtest.h>

#include <BibbleVM/vm.h>

#include <iostream>

using namespace bibblevm;

constexpr bool isAligned(void* ptr, size_t alignment) {
    return reinterpret_cast<uintptr_t>(ptr) % alignment == 0;
}

TEST(MemoryManager, BasicStringAllocation) {
    VM vm;

    oop::Object* s1 = vm.memoryManager().allocateString("hello");
    oop::Object* s2 = vm.memoryManager().allocateString("world");

    ASSERT_NE(s1, nullptr);
    ASSERT_NE(s2, nullptr);
    EXPECT_TRUE(isAligned(s1, 16));
    EXPECT_TRUE(isAligned(s2, 16));

    oop::StringObject* str1 = s1->asString();
    oop::StringObject* str2 = s2->asString();

    EXPECT_EQ(str1->lengthBytes, 5);
    EXPECT_EQ(str2->lengthBytes, 5);
    EXPECT_EQ(std::string(str1->bytes, str1->lengthBytes), "hello");
    EXPECT_EQ(std::string(str2->bytes, str2->lengthBytes), "world");
}

TEST(MemoryManager, NurseryCollectionWithRoots) {
    VM vm;

    std::vector<oop::Object*> roots;
    for (int i = 0; i < 100; i++) {
        oop::Object* obj = vm.memoryManager().allocateString("rooted");
        roots.push_back(obj);
        vm.memoryManager().addRoot(&roots.back());
    }

    for (int i = 0; i < 1000; i++) {
        vm.memoryManager().allocateString("ephemeral");
    }

    vm.memoryManager().safepoint(vm);

    for (auto* r : roots) {
        EXPECT_NE(r, nullptr);
        EXPECT_TRUE(isAligned(r, 16));
    }
}

TEST(MemoryManager, ForwardingAfterGC) {
    VM vm;

    oop::Object* obj = vm.memoryManager().allocateString("forwarding");
    vm.memoryManager().addRoot(&obj);

    for (int i = 0; i < 1000; i++) {
        vm.memoryManager().allocateString("dummy");
    }

    vm.memoryManager().safepoint(vm);

    ASSERT_NE(obj, nullptr);
    EXPECT_TRUE(isAligned(obj, 16));
    oop::StringObject* str = obj->asString();
    EXPECT_EQ(std::string(str->bytes, str->lengthBytes), "forwarding");
}

TEST(MemoryManager, MultipleRoots) {
    VM vm;

    std::vector<oop::Object*> objs;
    for (int i = 0; i < 50; i++) {
        objs.push_back(vm.memoryManager().allocateString("multiroot"));
    }
    vm.memoryManager().addRoot(objs);

    vm.memoryManager().safepoint(vm);

    for (auto* obj : objs) {
        EXPECT_NE(obj, nullptr);
        EXPECT_TRUE(isAligned(obj, 16));
    }
}

TEST(MemoryManager, StressAllocationAndCollection) {
    VM vm;
    std::vector<oop::Object*> roots;

    const int total = 100000000;
    for (int i = 0; i < total; i++) {
        oop::Object* obj = vm.memoryManager().allocateString("stress");
        if (i % 500 == 0) {
            roots.push_back(obj);
            vm.memoryManager().addRoot(&roots.back());
        }

        if (obj == nullptr) vm.memoryManager().safepoint(vm);
    }

    vm.memoryManager().safepoint(vm);

    for (auto* r : roots) {
        EXPECT_NE(r, nullptr);
    }
}