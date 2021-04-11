#include "cputests.h"

TEST_F(CpuTests, ASL_AccumulatorShiftsAndSetsCarryAndZero)
{
//    * = $1000
//    1000        LDA #$80        A9 80
//    1002        ASL A           0A

    uint8_t program[] = {
            0xA9, 0x80,
            0x0A
    };

    memory.WriteProgram(program);
    cpu.Reset();

    uint8_t cycles = 0;
    for (int i = 0; i < 2; i++)
    {
        cycles += cpu.Step();
    }

    ASSERT_EQ(cpu.a, 0x00);
    ASSERT_TRUE(cpu.s & (1 << 0)); // Carry
    ASSERT_TRUE(cpu.s & (1 << 1)); // Zero
    ASSERT_FALSE(cpu.s & (1 << 7)); // Negative should be set by LDA, but unset after.
}

TEST_F(CpuTests, ASL_ShiftsMemory)
{
#if 0
//    * = $1000
//    1000        LDA #$40        A9 40
//    1002        STA *$10        85 10
//    1004        ASL *$10        06 10

    uint8_t program[] = {
            0xA9, 0x40,
            0x85, 0x10,
            0x06, 0x10
    };
#else
//    * = $1000
//    1000        LDA #$40        A9 40
//    1002        STA $0010       8D 10 00
//    1005        ASL $0010       0E 10 00

    uint8_t program[] = {
            0xA9, 0x40,
            0x8D, 0x10, 0x00,
            0x0E, 0x10, 0x00,
    };
#endif
    memory.WriteProgram(program);
    cpu.Reset();

    uint8_t cycles = 0;
    for (int i = 0; i < 3; i++)
    {
        cycles += cpu.Step();
    }

    ASSERT_EQ(memory.Read(0x0010), 0x80);
    ASSERT_FALSE(cpu.s & (1 << 0)); // Carry
    ASSERT_FALSE(cpu.s & (1 << 1)); // Zero
    ASSERT_TRUE(cpu.s & (1 << 7));
}

TEST_F(CpuTests, ROL_AccumulatorWrapsAround)
{
//    * = $1000
//    1000        LDA #$80        A9 80
//    1002        ROL A           2A
//    1003        ROL A           2A

    uint8_t program[] = {
            0xA9, 0x80,
            0x2A,
            0x2A,
    };

    memory.WriteProgram(program);
    cpu.Reset();

    uint8_t cycles = 0;
    for (int i = 0; i < 3; i++)
    {
        cycles += cpu.Step();
    }

    ASSERT_EQ(cpu.a, 0x01);
    ASSERT_FALSE(cpu.s & (1 << 0)); // Carry
    ASSERT_FALSE(cpu.s & (1 << 1)); // Zero
    ASSERT_FALSE(cpu.s & (1 << 7)); // Negative
}

TEST_F(CpuTests, ROL_MemoryLocation)
{
//    * = $1000
//    1000        LDA #$AA        A9 AA
//    1002        STA *$10        85 10
//    1004        ROL *$10        26 10
//    1006        ROL *$10        26 10

    uint8_t program[] = {
            0xA9, 0xAA,
            0x85, 0x10,
            0x26, 0x10,
            0x26, 0x10,
    };

    memory.WriteProgram(program);
    cpu.Reset();

    uint8_t cycles = 0;
    for (int i = 0; i < 2; i++)
    {
        cycles += cpu.Step();
    }

    // TODO: Assert statuses too.
    ASSERT_EQ(memory.Read(0x10), 0xAA);
    cycles += cpu.Step();
    ASSERT_EQ(memory.Read(0x10), 0x54);
    cycles += cpu.Step();
    ASSERT_EQ(memory.Read(0x10), 0xA9);
    ASSERT_EQ(cycles, 15);
}