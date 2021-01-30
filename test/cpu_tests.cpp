#include "../src/cpu.h"
#include "../src/memory.h"
#include <gtest/gtest.h>

#if 0
TEST(CPUTests, LDAImmediate) {
    ASSERT_TRUE(false);
}

TEST(CPUTests, LDAAbsolute) {
    ASSERT_TRUE(false);
}
#endif

class TestMemory : public nes::Memory
{
public:
    uint8_t memory[0xFFFF] = { 0 };

    TestMemory()
    {
        memory[0xFFFC] = 0x00;
        memory[0xFFFD] = 0x10;
    }

    uint8_t Read(uint16_t address) override
    {
        return memory[address];
    }

    void Write(uint16_t address, uint8_t value) override
    {
        memory[address] = value;
    }
};

class CPUTests : public ::testing::Test {
public:
    nes::CPU cpu;
    TestMemory memory;

    CPUTests() : memory(), cpu(&memory)
    {
    }
};

TEST_F(CPUTests, WhenResetCpuStateSet)
{
    cpu.Reset();

    EXPECT_EQ(cpu.a, 0x00);
    EXPECT_EQ(cpu.x, 0x00);
    EXPECT_EQ(cpu.y, 0x00);
    EXPECT_EQ(cpu.pc, 0x1000);
    EXPECT_EQ(cpu.s, 0b00100000); // Unused bit should be the only one that's set
}

TEST_F(CPUTests, LDAImmediatePutsValueInARegister)
{
    cpu.memoryBus->Write(0x1000, 0xA1);
    cpu.memoryBus->Write(0x1001, 42);
    cpu.Reset();
    cpu.Step();

    EXPECT_EQ(cpu.a, 42);
    EXPECT_EQ(cpu.pc, 0x1000 + 2); // Instruction + 1 byte for absolute value
}

TEST_F(CPUTests, LDAAbsolutePutsValueInARegister)
{
    cpu.memoryBus->Write(0x1000, 0xAD);
    // Set the operand to 0x2000
    cpu.memoryBus->Write(0x1001, 0x00);
    cpu.memoryBus->Write(0x1002, 0x20);
    // Put 42 into an absolute address
    cpu.memoryBus->Write(0x2000, 42);
    cpu.Reset();
    cpu.Step();

    EXPECT_EQ(cpu.a, 42);
    EXPECT_EQ(cpu.pc, 0x1000 + 3); // Intruction + 2 bytes for address
}

TEST_F(CPUTests, NOPExecutes)
{
    cpu.memoryBus->Write(0x1000, 0xEA);
    cpu.Reset();
    cpu.Step();

    EXPECT_EQ(cpu.pc, 0x1000 + 1);
}

#if 0
class CpuIntructionTests : public ::testing::TestWithParam<int>
{
    nes::CPU cpu;
    TestMemory memory;

public:
    CpuIntructionTests() : memory(), cpu(&memory)
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