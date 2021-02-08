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
    // Picking low bytes of 0xd0 and 0x50 because the carry bit will be set
    // 0x50d0 + 0x5050 = 0xA120
    // 20688 + 20560 = 41248

    // https://www.masswerk.at/6502/assembler.html
    // * = $1000
    // 1000        LDA #$D0        A9 D0
    // 1002        ADC #$50        69 50
    // 1004        STA *$10        85 10
    // 1006        LDA #$50        A9 50
    // 1008        ADC #$50        69 50
    // 100A        STA *$11        85 11
    // done.
    uint8_t program[] = {
            0xA9, 0xD0,
            0x69, 0x50,
            0x85, 0x10,
            0xA9, 0x50,
            0x69, 0x50,
            0x85, 0x11
    };
    memory.WriteProgram(program);
    cpu.Reset();

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

TEST_F(CpuTests, SBC_Subtracts_Number)
{
//    * = $1000
//    1000        SEC             38
//    1001        LDA #$0A        A9 0A
//    1003        SBC #$05        E9 05
//    1005        STA *$14        85 14

    uint8_t program[] = {
            0x38,
            0xA9, 0x0A,
            0xE9, 0x05,
            0x85, 0x14
    };

    memory.WriteProgram(program);
    cpu.Reset();

    uint8_t cycles = 0;
    for (int i = 0; i < 4; i++)
    {
        cycles += cpu.Step();
    }

    auto result = memory.Read(0x14);
    EXPECT_EQ(result, 5);
}