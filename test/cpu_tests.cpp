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