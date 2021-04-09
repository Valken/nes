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