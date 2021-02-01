#include "cputests.h"

TEST_F(CpuTests, ADC_Adds_Operand_To_A_Register)
{
    cpu.Reset();
    cpu.a = 12;

    // ADC #15
    memory.Write(0x1000, 0x69);
    memory.Write(0x1001, 15);

    uint8_t cycles = cpu.Step();

    ASSERT_EQ(cpu.a, 12 + 15);
    ASSERT_EQ(cycles, 2);
}

// 0x50+0xd0=0x120
TEST_F(CpuTests, ADC_Large_Number_Sets_Carry_Bit)
{
    cpu.Reset();
    cpu.a = 0x50;

    memory.Write(0x1000, 0x69);
    memory.Write(0x1001, 0xd0);

    uint8_t cycles = cpu.Step();

    ASSERT_EQ(cpu.a, 0x20); // Answer should be 0x120
    ASSERT_TRUE(cpu.s & (1 << 0));
    ASSERT_EQ(cycles, 2);
}

TEST_F(CpuTests, ADC_Signed_Overflow_Sets_Overflow_Bit)
{
    cpu.Reset();
    cpu.a = 80;

    memory.Write(0x1000, 0x69);
    memory.Write(0x1001, 80);

    uint8_t cycles = cpu.Step();

    ASSERT_EQ(cpu.a, 160); // If we're considering A as signed, then this overflowed.
    ASSERT_TRUE(cpu.s & (1 << 6));
    ASSERT_EQ(cycles, 2);
}