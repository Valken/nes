#include "cputests.h"

TEST_F(CpuTests, TAX_Transfers_A_To_X)
{
    cpu.Reset();
    cpu.a = 0xdd;

    cpu.memoryBus->Write(0x1000, 0xAA);

    cpu.Step();

    EXPECT_EQ(cpu.x, 0xdd);
}

TEST_F(CpuTests, TAY_Transfers_A_To_Y)
{
    cpu.Reset();
    cpu.a = 0xdd;

    cpu.memoryBus->Write(0x1000, 0xA8);

    cpu.Step();

    EXPECT_EQ(cpu.y, 0xdd);
}

TEST_F(CpuTests, TXA_Of_Zero_Sets_Zero_Flag)
{
    cpu.Reset();
    cpu.x = 0;
    cpu.a = 1;

    cpu.memoryBus->Write(0x1000, 0x8A);

    cpu.Step();

    EXPECT_EQ(cpu.a, 0);
    EXPECT_TRUE(cpu.s & (1 << 1));
}

TEST_F(CpuTests, TYA_Of_Negative_Number_Sets_Negative_Flag)
{
    cpu.Reset();
    cpu.y = (uint8_t)-1;
    cpu.a = 1;

    cpu.memoryBus->Write(0x1000, 0x98);

    cpu.Step();

    EXPECT_EQ(cpu.a, (uint8_t)-1);
    EXPECT_TRUE(cpu.s & (1 << 7));
}