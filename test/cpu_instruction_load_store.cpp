#include "cputests.h"

TEST_F(CpuTests, LDA_Immediate_Sets_A)
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

TEST_F(CpuTests, LDA_ZeroPage_Sets_A)
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

TEST_F(CpuTests, LDA_ZeroPageX_Sets_A)
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

TEST_F(CpuTests, LDA_ZeroPageX_Address_Wraps)
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

TEST_F(CpuTests, LDX_ZeroPageY_Sets_X)
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

TEST_F(CpuTests, LDA_Absolute_Sets_A)
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
EXPECT_EQ(cpu.pc, 0x1000 + 3); // Instruction + 2 bytes for address
}

TEST_F(CpuTests, LDA_AbsoluteX_Sets_A_PageCrossed_Cycle_Occurs)
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

TEST_F(CpuTests, LDA_AbsoluteY_Sets_A)
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

TEST_F(CpuTests, LDA_IndexedIndirect_Sets_A)
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

TEST_F(CpuTests, LDA_IndirectIndexed_Sets_A)
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

TEST_F(CpuTests, LDA_Of_Zero_Sets_Zero_Flag)
{
cpu.memoryBus->Write(0x1000, 0xA9);
cpu.memoryBus->Write(0x1001, 0);
cpu.Reset();
uint8_t flags = cpu.s;
cpu.Step();

EXPECT_TRUE(flags ^ cpu.s); // Flags should have changed
EXPECT_TRUE(cpu.s & (1 << 1));
}

TEST_F(CpuTests, LDA_Of_Negative_Value_Sets_Negative_Flag)
{
cpu.memoryBus->Write(0x1000, 0xA9);
cpu.memoryBus->Write(0x1001, (uint8_t)-1);
cpu.Reset();
cpu.Step();

EXPECT_TRUE(cpu.s & (1 << 7));
}

// STA

TEST_F(CpuTests, STA_ZeroPage_Writes_A)
{
    // LDA #42
    cpu.memoryBus->Write(0x1000, 0xA9);
    cpu.memoryBus->Write(0x1001, 42);

    cpu.memoryBus->Write(0x1002, 0x85);
    cpu.memoryBus->Write(0x1003, 0x7F);

    uint8_t cycles = 0;

    cpu.Reset();
    cycles += cpu.Step();
    cycles += cpu.Step();

    EXPECT_EQ(memory.Read(0x007F), 42);
    EXPECT_EQ(cycles, 2 + 3);
}

TEST_F(CpuTests, STA_ZeroPageX_Writes_A)
{
    // LDA #42
    cpu.memoryBus->Write(0x1000, 0xA9);
    cpu.memoryBus->Write(0x1001, 42);

    // LDX #$FF
    cpu.memoryBus->Write(0x1002, 0xA2);
    cpu.memoryBus->Write(0x1003, 0xFF);

    // STA #$80,X
    cpu.memoryBus->Write(0x1004, 0x95);
    cpu.memoryBus->Write(0x1005, 0x80);

    uint8_t cycles = 0;

    cpu.Reset();
    cycles += cpu.Step();
    cycles += cpu.Step();
    cycles += cpu.Step();

    EXPECT_EQ(memory.Read(0x007F), 42);
    EXPECT_EQ(cycles, 2 + 2 + 4);
}

TEST_F(CpuTests, STA_Absolute_Writes_A)
{
    cpu.memoryBus->Write(0x1000, 0xA9);
    cpu.memoryBus->Write(0x1001, 42);
    cpu.memoryBus->Write(0x1002, 0x8D);
    cpu.memoryBus->Write(0x1003, 0x34);
    cpu.memoryBus->Write(0x1004, 0x12);

    uint8_t cycles = 0;

    cpu.Reset();
    cycles += cpu.Step();
    cycles += cpu.Step();

    EXPECT_EQ(memory.Read(0x1234), 42);
    EXPECT_EQ(cycles, 2 + 4);
}

TEST_F(CpuTests, STA_AbsoluteX_Writes_A)
{
    cpu.memoryBus->Write(0x1000, 0xA9);
    cpu.memoryBus->Write(0x1001, 42);
    cpu.memoryBus->Write(0x1002, 0xA2);
    cpu.memoryBus->Write(0x1003, 0x01);
    cpu.memoryBus->Write(0x1004, 0x9D);
    cpu.memoryBus->Write(0x1005, 0x34);
    cpu.memoryBus->Write(0x1006, 0x12);

    uint8_t cycles = 0;

    cpu.Reset();
    cycles += cpu.Step();
    cycles += cpu.Step();
    cycles += cpu.Step();

    EXPECT_EQ(memory.Read(0x1235), 42);
    EXPECT_EQ(cycles, 2 + 2 + 5);
}

TEST_F(CpuTests, STA_AbsoluteY_Writes_A)
{
    cpu.memoryBus->Write(0x1000, 0xA9);
    cpu.memoryBus->Write(0x1001, 42);
    cpu.memoryBus->Write(0x1002, 0xA0);
    cpu.memoryBus->Write(0x1003, 0x01);
    cpu.memoryBus->Write(0x1004, 0x99);
    cpu.memoryBus->Write(0x1005, 0x34);
    cpu.memoryBus->Write(0x1006, 0x12);

    uint8_t cycles = 0;

    cpu.Reset();
    cycles += cpu.Step();
    cycles += cpu.Step();
    cycles += cpu.Step();

    EXPECT_EQ(memory.Read(0x1235), 42);
    EXPECT_EQ(cycles, 2 + 2 + 5);
}

TEST_F(CpuTests, STA_IndexedIndirect_Writes_A)
{
    cpu.memoryBus->Write(0x1000, 0xA9);
    cpu.memoryBus->Write(0x1001, 42);
    cpu.memoryBus->Write(0x1002, 0xA2);
    cpu.memoryBus->Write(0x1003, 0x04);
    cpu.memoryBus->Write(0x1004, 0x81);
    cpu.memoryBus->Write(0x1005, 0x20);

    cpu.memoryBus->Write(0x0024, 0x74);
    cpu.memoryBus->Write(0x0025, 0x20);


    uint8_t cycles = 0;

    cpu.Reset();
    cycles += cpu.Step();
    cycles += cpu.Step();
    cycles += cpu.Step();

    EXPECT_EQ(cpu.memoryBus->Read(0x2074), 42);
    EXPECT_EQ(cycles, 2 + 2 + 6);
}

TEST_F(CpuTests, STA_IndirectIndexed_Writes_A)
{
    cpu.memoryBus->Write(0x1000, 0xA9);
    cpu.memoryBus->Write(0x1001, 42);
    cpu.memoryBus->Write(0x1002, 0xA0);
    cpu.memoryBus->Write(0x1003, 0x10);
    cpu.memoryBus->Write(0x1004, 0x91);
    cpu.memoryBus->Write(0x1005, 0x86);

    cpu.memoryBus->Write(0x0086, 0x28);
    cpu.memoryBus->Write(0x0087, 0x40);
    cpu.memoryBus->Write(0x4038, 42);

    uint8_t cycles = 0;

    cpu.Reset();
    cycles += cpu.Step();
    cycles += cpu.Step();
    cycles += cpu.Step();

    EXPECT_EQ(cpu.memoryBus->Read(0x4038), 42);
    EXPECT_EQ(cycles, 2 + 2 + 6);
}

TEST_F(CpuTests, STX_ZeroPage_Writes_X)
{
    cpu.Reset();
    cpu.x = 42;

    cpu.memoryBus->Write(0x1000, 0x86);
    cpu.memoryBus->Write(0x1001, 0x12);

    cpu.Step();

    EXPECT_EQ(cpu.memoryBus->Read(0x0012), 42);
}

TEST_F(CpuTests, STY_ZeroPageX_Writes_Y)
{
    cpu.Reset();
    cpu.x = 10;
    cpu.y = 42;

    cpu.memoryBus->Write(0x1000, 0x94);
    cpu.memoryBus->Write(0x1001, 0x12);

    cpu.Step();

    EXPECT_EQ(cpu.memoryBus->Read(0x0012 + 10), 42);
}