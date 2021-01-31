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
    uint8_t data[0xFFFF] = { 0 };

    TestMemory()
    {
        // Set the program start address to 0x1000
        // Little endian so least significant bit first
        data[0xFFFC] = 0x00;
        data[0xFFFD] = 0x10;
    }

    uint8_t Read(uint16_t address) override
    {
        return data[address];
    }

    void Write(uint16_t address, uint8_t value) override
    {
        data[address] = value;
    }
};

class CPUTests : public ::testing::Test
        {
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
    cpu.memoryBus->Write(0x1000, 0xA9);
    cpu.memoryBus->Write(0x1001, 42);
    cpu.Reset();
    uint8_t flags = cpu.s;
    cpu.Step();

    EXPECT_EQ(cpu.a, 42);
    EXPECT_TRUE(!(flags ^ cpu.s)); // Flags should be unaffected
    EXPECT_EQ(cpu.pc, 0x1000 + 2); // Instruction + 1 byte for absolute value
}

TEST_F(CPUTests, LDAZeroPagePutsValueInRegister)
{
    cpu.memoryBus->Write(0x1000, 0xA5);
    cpu.memoryBus->Write(0x1001, 0x10);
    cpu.memoryBus->Write(0x0010, 42);
    cpu.Reset();
    uint8_t flags = cpu.s;
    cpu.Step();

    EXPECT_EQ(cpu.a, 42);
    EXPECT_TRUE(!(flags ^ cpu.s)); // Flags should be unaffected
    EXPECT_EQ(cpu.pc, 0x1000 + 2); // Instruction + § byte for absolute value
}

TEST_F(CPUTests, LDAZeroPageXPutsValueInRegister)
{
    // LDX #5
    cpu.memoryBus->Write(0x1000, 0xA2);
    cpu.memoryBus->Write(0x1001, 5);

    // LDA $10,X
    cpu.memoryBus->Write(0x1002, 0xB5);
    cpu.memoryBus->Write(0x1003, 0x10);

    // Put 42 into the address of instruction above, offset by the value we write to X register
    cpu.memoryBus->Write(0x0015, 42);

    cpu.Reset();
    cpu.Step();
    cpu.Step();

    EXPECT_EQ(cpu.x, 5);
    EXPECT_EQ(cpu.a, 42);
}

TEST_F(CPUTests, LDAZeroPageXAddressWraps)
{
    // LDX #$FF
    cpu.memoryBus->Write(0x1000, 0xA2);
    cpu.memoryBus->Write(0x1001, 0xFF);

    // LDA $80,X
    cpu.memoryBus->Write(0x1002, 0xB5);
    cpu.memoryBus->Write(0x1003, 0x80);

    // Put 42 into the address of instruction above, offset by the value we write to X register
    cpu.memoryBus->Write(0x007F, 42);

    cpu.Reset();
    cpu.Step();
    cpu.Step();

    EXPECT_EQ(cpu.x, 0xFF);
    EXPECT_EQ(cpu.a, 42);
}

TEST_F(CPUTests, LDXZeroPageYPutsValueInRegister)
{
    // LDY #5
    cpu.memoryBus->Write(0x1000, 0xA0);
    cpu.memoryBus->Write(0x1001, 5);

    // LDX $10,Y
    cpu.memoryBus->Write(0x1002, 0xB6);
    cpu.memoryBus->Write(0x1003, 0x10);

    // Put 42 into the address of instruction above, offset by the value we write to X register
    cpu.memoryBus->Write(0x0015, 42);

    cpu.Reset();
    cpu.Step();
    cpu.Step();

    EXPECT_EQ(cpu.y, 5);
    EXPECT_EQ(cpu.x, 42);
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

TEST_F(CPUTests, LDAAbsoluteXPutsValueInARegisterAndPageCrosses)
{
    cpu.memoryBus->Write(0x1000, 0xA2);
    cpu.memoryBus->Write(0x1001, 0x01);

    // This address was chosen so that adding the 1 in x register means memory page crossed.
    // This results in an extra cycle which we test for below
    cpu.memoryBus->Write(0x1002, 0xBD);
    cpu.memoryBus->Write(0x1003, 0xFF);
    cpu.memoryBus->Write(0x1004, 0x00);

    cpu.memoryBus->Write(0x0100, 42);

    uint8_t cycles = 0;

    cpu.Reset();
    cycles += cpu.Step();
    cycles += cpu.Step();

    EXPECT_EQ(cpu.a, 42);
    EXPECT_EQ(cycles, 2 + 4 + 1);
}

TEST_F(CPUTests, LDAAbsoluteYPutsValueInARegister)
{
    cpu.memoryBus->Write(0x1000, 0xA0);
    cpu.memoryBus->Write(0x1001, 0x89);

    cpu.memoryBus->Write(0x1002, 0xB9);
    cpu.memoryBus->Write(0x1003, 0x00);
    cpu.memoryBus->Write(0x1004, 0x20);

    cpu.memoryBus->Write(0x2089, 42);

    uint8_t cycles = 0;

    cpu.Reset();
    cycles += cpu.Step();
    cycles += cpu.Step();

    EXPECT_EQ(cpu.a, 42);
    EXPECT_EQ(cycles, 2 + 4);
}

TEST_F(CPUTests, LDAIndexedIndirectPutsValueInARegister)
{
    // LDX #$04
    cpu.memoryBus->Write(0x1000, 0xA2);
    cpu.memoryBus->Write(0x1001, 0x04);

    cpu.memoryBus->Write(0x1002, 0xA1);
    cpu.memoryBus->Write(0x1003, 0x20);

    cpu.memoryBus->Write(0x0024, 0x74);
    cpu.memoryBus->Write(0x0025, 0x20);

    cpu.memoryBus->Write(0x2074, 42);

    uint8_t cycles = 0;

    cpu.Reset();
    cycles += cpu.Step();
    cycles += cpu.Step();

    EXPECT_EQ(cpu.a, 42);
    EXPECT_EQ(cycles, 2 + 6);
}

TEST_F(CPUTests, LDAIndirectIndexedtPutsValueInARegister)
{
    // LDY #$10
    cpu.memoryBus->Write(0x1000, 0xA0);
    cpu.memoryBus->Write(0x1001, 0x10);

    // LDA ($86),Y
    cpu.memoryBus->Write(0x1002, 0xB1);
    cpu.memoryBus->Write(0x1003, 0x86);

    //  Set up memory
    cpu.memoryBus->Write(0x0086, 0x28);
    cpu.memoryBus->Write(0x0087, 0x40);
    cpu.memoryBus->Write(0x4038, 42);

    uint8_t cycles = 0;

    cpu.Reset();
    cycles += cpu.Step();
    cycles += cpu.Step();

    EXPECT_EQ(cpu.a, 42);
    EXPECT_EQ(cycles, 2 + 5 /* page crosses is a thing here... */);
}

TEST_F(CPUTests, LDAOfZeroSetsZeroFlag)
{
    cpu.memoryBus->Write(0x1000, 0xA9);
    cpu.memoryBus->Write(0x1001, 0);
    cpu.Reset();
    uint8_t flags = cpu.s;
    cpu.Step();

    EXPECT_TRUE(flags ^ cpu.s); // Flags should have changed
    EXPECT_TRUE(cpu.s & (1 << 1));
}

TEST_F(CPUTests, LDAOfNegativeValueSetsNegativeFlag)
{
    cpu.memoryBus->Write(0x1000, 0xA9);
    cpu.memoryBus->Write(0x1001, (uint8_t)-1);
    cpu.Reset();
    cpu.Step();

    EXPECT_TRUE(cpu.s & (1 << 7));
}

// STA

TEST_F(CPUTests, STAZeroPageWritesToMemory)
{

}

// NOP

TEST_F(CPUTests, NOPExecutes)
{
    cpu.memoryBus->Write(0x1000, 0xEA);
    cpu.Reset();
    cpu.Step();

    EXPECT_EQ(cpu.pc, 0x1000 + 1);
}

// JMP

TEST_F(CPUTests, JMPAbsoluteSetsProgramCounter)
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

TEST_F(CPUTests, JMPIndirectSetsProgramCounter)
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

TEST_F(CPUTests, JMPIndirectSetsProgramCounterBug)
{
    // 0x0022 contains a pointer to 0x2000
    cpu.memoryBus->Write(0x1000, 0x6C);
    cpu.memoryBus->Write(0x1001, 0xFF);
    cpu.memoryBus->Write(0x1002, 0x01);
    cpu.memoryBus->Write(0x01FF, 0x00);
    cpu.memoryBus->Write(0x0100, 0x20);

    cpu.Reset();
    uint8_t cycles = 0;
    uint8_t flags = cpu.s;
    cycles += cpu.Step();

    EXPECT_TRUE(!(flags ^ cpu.s));
    EXPECT_EQ(cpu.pc, 0x2000);
    EXPECT_EQ(cycles, 5);
}

#if 0
class CpuIntructionTests : public ::testing::TestWithParam<int>
{
    nes::CPU cpu;
    TestMemory data;

public:
    CpuIntructionTests() : data(), cpu(&data)
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