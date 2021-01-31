#include "cputests.h"

TEST_F(CpuTests, JMP_Absolute_Sets_ProgramCounter)
{
    cpu.memoryBus->Write(0x1000, 0x4C);
    cpu.memoryBus->Write(0x1001, 0x21);
    cpu.memoryBus->Write(0x1002, 0x10);

    cpu.Reset();
    uint8_t cycles = 0;
    uint8_t flags = cpu.s;
    cycles += cpu.Step();

    EXPECT_TRUE(!(flags ^ cpu.s));
    EXPECT_EQ(cpu.pc, 0x1021);
    EXPECT_EQ(cycles, 3);
}

TEST_F(CpuTests, JMP_Indirect_Sets_ProgramCounter)
{
    // 0x0022 contains a pointer to 0x2000
    cpu.memoryBus->Write(0x1000, 0x6C);
    cpu.memoryBus->Write(0x1001, 0x22);
    cpu.memoryBus->Write(0x1002, 0x00);
    cpu.memoryBus->Write(0x0022, 0x00);
    cpu.memoryBus->Write(0x0023, 0x20);

    cpu.Reset();
    uint8_t cycles = 0;
    uint8_t flags = cpu.s;
    cycles += cpu.Step();

    EXPECT_TRUE(!(flags ^ cpu.s));
    EXPECT_EQ(cpu.pc, 0x2000);
    EXPECT_EQ(cycles, 5);
}

TEST_F(CpuTests, JMP_Indirect_Sets_ProgramCounter_Bug)
{
    // 0x0022 contains a pointer to 0x2000
    cpu.memoryBus->Write(0x1000, 0x6C);
    cpu.memoryBus->Write(0x1001, 0xFF);
    cpu.memoryBus->Write(0x1002, 0x01); // 0x01FF points to actual address we want
    cpu.memoryBus->Write(0x01FF, 0x00);
    cpu.memoryBus->Write(0x0100, 0x20); // MSB of the address pointed to by 0x01FF isn't in 0x200, but wraps to state of 0x01xx page

    cpu.Reset();
    uint8_t cycles = 0;
    uint8_t flags = cpu.s;
    cycles += cpu.Step();

    EXPECT_TRUE(!(flags ^ cpu.s));
    EXPECT_EQ(cpu.pc, 0x2000);
    EXPECT_EQ(cycles, 5);
}