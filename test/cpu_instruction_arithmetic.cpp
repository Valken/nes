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

TEST_F(CpuTests, ADC_Can_Add_16_Bit_Number_With_Carry)
{
    cpu.Reset();

    // Picking low bytes of 0xd0 and 0x50 because the carry bit will be set
    // 0x50d0 + 0x5050 = 0xA120
    // 20688 + 20560 = 41248

    // https://www.masswerk.at/6502/assembler.html
    // LDA #$d0
    // ADC #$50
    // STA *$10
    // LDA #$50
    // ADC #$50
    // STA *$11
    // I need a better way of loading simple programs!
    memory.Write(0x1000, 0xA9);
    memory.Write(0x1001, 0xd0);
    memory.Write(0x1002, 0x69);
    memory.Write(0x1003, 0x50);
    memory.Write(0x1004, 0x85);
    memory.Write(0x1005, 0x10);
    memory.Write(0x1006, 0xA9);
    memory.Write(0x1007, 0x50);
    memory.Write(0x1008, 0x69);
    memory.Write(0x1009, 0x50);
    memory.Write(0x100A, 0x85);
    memory.Write(0x100B, 0x11);

    uint8_t cycles = 0;
    for (int i = 0; i < 6; i++)
    {
        cycles += cpu.Step();
    }

    auto low = memory.Read(0x10); // Should be 20
    auto high = memory.Read(0x11); // Should be A0 + 1
    uint16_t answer = high << 8 | low;

    ASSERT_EQ(answer, 41248);
}