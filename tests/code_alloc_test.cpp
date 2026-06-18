// Copyright 2026 Jannik Laugmand Bülow

#include <BibbleVM/allocator/code_alloc.h>

#include <gtest/gtest.h>

#include <cstring>

using namespace bibblevm;
using namespace bibblevm::compiler;

class CodeAllocatorTest : public ::testing::Test {
protected:
    CodeAllocator allocator;
};

TEST_F(CodeAllocatorTest, AllocateReturnsValidPointer) {
    constexpr size_t size = 64;

    Code* code = allocator.allocate(size);

    ASSERT_NE(code, nullptr);
    EXPECT_NE(code->mc, nullptr);
    EXPECT_EQ(code->size, size);

    allocator.deallocate(code);
}

TEST_F(CodeAllocatorTest, MultipleAllocationsAreDistinct) {
    Code* code1 = allocator.allocate(64);
    Code* code2 = allocator.allocate(64);

    ASSERT_NE(code1, nullptr);
    ASSERT_NE(code2, nullptr);

    EXPECT_NE(code1, code2);
    EXPECT_NE(code1->mc, code2->mc);

    allocator.deallocate(code1);
    allocator.deallocate(code2);
}

TEST_F(CodeAllocatorTest, MarkExecutableAndReadWriteDoNotCrash) {
    Code* code = allocator.allocate(64);

    ASSERT_NE(code, nullptr);

    allocator.markExecutable(code);
    allocator.markReadWrite(code);
    allocator.markExecutable(code);

    allocator.deallocate(code);
}

TEST_F(CodeAllocatorTest, GeneratedCodeCanBeExecuted) {
    Code* code = allocator.allocate(16);

    ASSERT_NE(code, nullptr);
    ASSERT_NE(code->mc, nullptr);

#if defined(__x86_64__) || defined(_M_X64)

    // mov eax, 42
    // ret
    const unsigned char machineCode[] = {
        0xB8, 0x2A, 0x00, 0x00, 0x00,
        0xC3
    };

    std::memcpy(code->mc, machineCode, sizeof(machineCode));

    allocator.markExecutable(code);

    using Function = int (*)();
    auto fn = reinterpret_cast<Function>(code->mc);

    EXPECT_EQ(fn(), 42);

    allocator.markReadWrite(code);

#else
    GTEST_SKIP() << "Machine code execution test only implemented for x86-64.";
#endif

    allocator.deallocate(code);
}

TEST_F(CodeAllocatorTest, CanSwitchBackToReadWriteAfterExecution) {
    Code* code = allocator.allocate(16);

    ASSERT_NE(code, nullptr);

    allocator.markExecutable(code);
    allocator.markReadWrite(code);

    unsigned char* bytes = static_cast<unsigned char*>(code->mc);

    EXPECT_NO_THROW(bytes[0] = 0x90); // NOP

    allocator.deallocate(code);
}