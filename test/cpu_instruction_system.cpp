#include "cputests.h"

TEST_F(CpuTests, NOP_Executes)
{
    cpu.memoryBus->Write(0x1000, 0xEA);
    cpu.Reset();
    cpu.Step();

    EXPECT_EQ(cpu.pc, 0x1000 + 1);
}
