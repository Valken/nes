#include "cputests.h"

TEST_F(CpuTests, Reset_Tests_State)
{
    cpu.Reset();

    EXPECT_EQ(cpu.a, 0x00);
    EXPECT_EQ(cpu.x, 0x00);
    EXPECT_EQ(cpu.y, 0x00);
    EXPECT_EQ(cpu.pc, 0x1000);
    EXPECT_EQ(cpu.s, 0b00100000); // Unused bit should be the only one that's set
}

TEST(StackTest, Push)
{
    TestMemory memory;
    nes::CPU cpu(&memory);
    cpu.Reset();

    cpu.Push(42);

    EXPECT_EQ(cpu.sp, 0xFD - 1);

    cpu.Push(11);
    EXPECT_EQ(cpu.sp, 0xFD - 2);

    EXPECT_EQ(memory.Read(0x1FD), 42);
}

TEST(StackTest, Pop)
{
    TestMemory memory;
    nes::CPU cpu(&memory);
    cpu.Reset();

    cpu.Push(42);
    auto value = cpu.Pop();

    EXPECT_EQ(cpu.sp, 0xFD);
    EXPECT_EQ(value, 42);
}

#if 0
class CpuIntructionTests : public ::testing::TestWithParam<int>
{
    nes::CPU cpu;
    TestMemory data;

public:
    CpuIntructionTests() : data(), cpu(&data)
    {
    }
};

TEST_P(CpuIntructionTests, LDA)
{
    auto param = GetParam();
    ASSERT_GT(param, 0);
}

INSTANTIATE_TEST_SUITE_P(Things, CpuIntructionTests, ::testing::Values(1, 2, 3, 4, -1));
#endif