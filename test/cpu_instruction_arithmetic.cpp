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
    EXPECT_TRUE(cpu.s & (1 << 0));
    ASSERT_FALSE(cpu.s & (1 << 6)); // Should be no signed overflow
}

TEST_F(CpuTests, SBC_Signed_Overflow_Sets_Overflow_Bit)
{

//    * = $1000
//    1000        SEC             38
//    1001        LDA #$50        A9 50
//    1003        SBC #$B0        E9 B0
//    1005        STA *$14        85 14

    uint8_t program[] = {
            0x38,
            0xA9, 0x50,
            0xE9, 0xB0,
            0x85, 0x14,
    };

    memory.WriteProgram(program);
    cpu.Reset();

    uint8_t cycles = 0;
    for (int i = 0; i < 4; i++)
    {
        cycles += cpu.Step();
    }

    auto result = memory.Read(0x14);
    EXPECT_EQ(result, static_cast<uint8_t>(-96));
    ASSERT_FALSE(cpu.s & (1 << 0)); // Borrow
    ASSERT_TRUE(cpu.s & (1 << 7)); // Negative
    ASSERT_TRUE(cpu.s & (1 << 6)); // Signed overflow
}

TEST_F(CpuTests, SBC_Can_Subtract_16_Bit_Number_With_Borrow)
{
//    * = $1000
//    1000        SEC             38
//    1001        LDA #$50        A9 50
//    1003        SBC #$F0        E9 F0
//    1005        STA *$14        85 14
//    1007        LDA #$50        A9 50
//    1009        SBC #$F0        E9 F0
//    100B        STA *$15        85 15

    uint8_t program[] = {
            0x38,
            0xA9, 0x50,
            0xE9, 0xF0,
            0x85, 0x10,
            0xA9, 0x50,
            0xE9, 0xF0,
            0x85, 0x11,
    };

    memory.WriteProgram(program);
    cpu.Reset();

    uint8_t cycles = 0;
    for (int i = 0; i < 7; i++)
    {
        cycles += cpu.Step();
    }

    auto low = memory.Read(0x10); // Should be 20
    auto high = memory.Read(0x11); // Should be A0 + 1
    uint16_t answer = high << 8 | low;

    ASSERT_EQ(answer, 24416);
}

TEST_F(CpuTests, CMP_SameValue_Sets_Z_And_C)
{
    uint8_t program[] = {
        0xA9, 0x50,
        0xC9, 0x50
    };

    memory.WriteProgram(program);
    cpu.Reset();

    uint8_t cycles = 0;
    for (int i = 0; i < 2; i++)
    {
        cycles += cpu.Step();
    }

    ASSERT_TRUE(cpu.s & 1 << 0);
    ASSERT_TRUE(cpu.s & 1 << 1);
    ASSERT_FALSE(cpu.s & 1 << 7);
}

TEST_F(CpuTests, CMP_GreaterValue_Sets_C)
{
    uint8_t program[] = {
            0xA9, 0x60,
            0xC9, 0x50
    };

    memory.WriteProgram(program);
    cpu.Reset();

    uint8_t cycles = 0;
    for (int i = 0; i < 2; i++)
    {
        cycles += cpu.Step();
    }

    ASSERT_TRUE(cpu.s & 1 << 0);
    ASSERT_FALSE(cpu.s & 1 << 1);
    ASSERT_FALSE(cpu.s & 1 << 7);
}

TEST_F(CpuTests, CMP_LessValue_Sets_N)
{
    uint8_t program[] = {
            0xA9, 0x40,
            0xC9, 0x50
    };

    memory.WriteProgram(program);
    cpu.Reset();

    uint8_t cycles = 0;
    for (int i = 0; i < 2; i++)
    {
        cycles += cpu.Step();
    }

    ASSERT_FALSE(cpu.s & 1 << 0);
    ASSERT_FALSE(cpu.s & 1 << 1);
    ASSERT_TRUE(cpu.s & 1 << 7);
}

TEST_F(CpuTests, INC_IncrementsMemoryAtAddress)
{
//    * = $1000
//    1000        LDA #$40        A9 40
//    1002        STA *$14        85 14
//    1004        INC *$14        E6 14

    uint8_t program[] = {
            0xA9, 0x40,
            0x85, 0x14,
            0xE6, 0x14,
    };

    memory.WriteProgram(program);
    cpu.Reset();

    uint8_t cycles = 0;
    for (int i = 0; i < 3; i++)
    {
        cycles += cpu.Step();
    }

    ASSERT_EQ(memory.Read(0x14), 0x41);
}

TEST_F(CpuTests, INC_ZeroFlagGetsSet)
{
    uint8_t program[] = {
            0xA9, 0xFF, // FF so should wrap around
            0x85, 0x14,
            0xE6, 0x14,
    };

    memory.WriteProgram(program);
    cpu.Reset();

    uint8_t cycles = 0;
    for (int i = 0; i < 3; i++)
    {
        cycles += cpu.Step();
    }

    ASSERT_TRUE(cpu.s & 1 << 1);
    ASSERT_FALSE(cpu.s & 1 << 7);
}


TEST_F(CpuTests, INC_NegativeFlagGetsSet)
{
    uint8_t program[] = {
            0xA9, 0x7F, // Will increment into negative range of signed 8 bit integer
            0x85, 0x14,
            0xE6, 0x14,
    };

    memory.WriteProgram(program);
    cpu.Reset();

    uint8_t cycles = 0;
    for (int i = 0; i < 3; i++)
    {
        cycles += cpu.Step();
    }

    ASSERT_FALSE(cpu.s & 1 << 1);
    ASSERT_TRUE(cpu.s & 1 << 7);
}

TEST_F(CpuTests, INX_IncrementsXRegister)
{
//    * = $1000
//    1000        LDX #$7F        A2 7F
//    1002        INX             E8
    uint8_t program[] = {
            0xA2, 0x7F,
            0xE8
    };

    memory.WriteProgram(program);
    cpu.Reset();

    uint8_t cycles = 0;
    for (int i = 0; i < 2; i++)
    {
        cycles += cpu.Step();
    }

    ASSERT_EQ(cpu.x, static_cast<uint8_t >(-128));
    ASSERT_TRUE(cpu.s & 1 << 7);
}

TEST_F(CpuTests, INY_IncrementsYRegister)
{
//    * = $1000
//    1000        LDY #$FF        A0 FF
//    1002        INY             C8

    uint8_t program[] = {
            0xA0, 0xFF,
            0xC8
    };

    memory.WriteProgram(program);
    cpu.Reset();

    uint8_t cycles = 0;
    for (int i = 0; i < 2; i++)
    {
        cycles += cpu.Step();
    }

    ASSERT_EQ(cpu.x, 0);
    ASSERT_TRUE(cpu.s & 1 << 1);
}

TEST_F(CpuTests, DEC_DecrementsMemoryAtAddress)
{
//    * = $1000
//    1000        LDA #$40        A9 40
//    1002        STA *$14        85 14
//    1004        DEC *$14        C6 14

    uint8_t program[] = {
            0xA9, 0x40,
            0x85, 0x14,
            0xC6, 0x14,
    };

    memory.WriteProgram(program);
    cpu.Reset();

    uint8_t cycles = 0;
    for (int i = 0; i < 3; i++)
    {
        cycles += cpu.Step();
    }

    ASSERT_EQ(memory.Read(0x14), 0x3F);
}