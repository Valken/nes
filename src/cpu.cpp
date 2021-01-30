#include "cpu.h"
#include "memory.h"
#include <functional>
#include <cassert>

enum CpuFlags
{
    C = (1 << 0), // Carry
    Z = (1 << 1), // Zero
    I = (1 << 2), // Interrupt
    D = (1 << 3), // Decimal
    B = (1 << 4), // Break
    U = (1 << 5), // Unused
    V = (1 << 6), // Overflow
    N = (1 << 7), // Negative
};

static uint8_t SetZN(uint8_t flags, uint8_t value)
{
    if (value == 0) flags |= Z;
    if (value & 0x80) flags |= N;
    return  flags;
}

namespace nes
{

CPU::CPU(Memory* const memoryBus) : pc(0x0000), a(0), x(0), y(0), s(0x00), memoryBus(memoryBus)
{
}

void CPU::Step()
{
    // Interrupt?

    auto instruction = Fetch();
    auto instructionInfo = InstructionInfo[instruction];
    auto operand = Decode(instructionInfo.addressMode);
    std::invoke(instructionInfo.instruction, this, operand);
    pc += instructionInfo.instructionSize;

    // Return num cycles
}

void CPU::Reset()
{
    a = 0x00;
    x = 0x00;
    y = 0x00;

    s = 0x00 | U;

    uint16_t const resetVector = 0xFFFC;
    pc = Read16(resetVector);

    sp = 0xFD;
}

uint8_t CPU::Fetch()
{
    return memoryBus->Read(pc);
}

Operand CPU::Decode(AddressMode addressMode) const
{
    uint16_t address = 0x00;
    
    switch (addressMode)
    {
        case AddressMode::Implicit:
            address = 0;
            break;
            
        case AddressMode::Accumulator:
            address = 0;
            break;
            
        case AddressMode::Immediate:
            address = pc + 1;
            break;
            
        case AddressMode::ZeroPage:
            address = memoryBus->Read(pc + 1);
            break;
            
        case AddressMode::ZeroPageX:
            address = (memoryBus->Read(pc + 1) + x) & 0xFF;
            break;
            
        case AddressMode::ZeroPageY:
            address = (memoryBus->Read(pc + 1) + y) & 0xFF;
            break;
            
        case AddressMode::Relative:
            break;

        case AddressMode::Absolute:
            address = Read16(pc + 1);
            break;

        case AddressMode::AbsoluteX:
        case AddressMode::AbsoluteY:
        case AddressMode::Indirect:
        case AddressMode::IndexedIndirect:
        case AddressMode::IndirectIndexed:
        default:
            assert(false);
            break;
    }
    
    return Operand { .address = address, .addressMode = addressMode };
}

uint16_t CPU::Read16(uint16_t address) const
{
    uint16_t lo = memoryBus->Read(address);
    uint16_t hi = memoryBus->Read(address + 1);

    return (hi << 8) | lo;
}

InstructionInfo CPU::InstructionInfo[256] =
{
    /* 0x00 */ {},
    /* 0x01 */ {},
    /* 0x02 */ {},
    /* 0x03 */ {},
    /* 0x04 */ {},
    /* 0x05 */ {},
    /* 0x06 */ {},
    /* 0x07 */ {},
    /* 0x08 */ {},
    /* 0x09 */ {},
    /* 0x0A */ {},
    /* 0x0B */ {},
    /* 0x0C */ {},
    /* 0x0D */ {},
    /* 0x0E */ {},
    /* 0x0F */ {},

    /* 0x10 */ {},
    /* 0x11 */ {},
    /* 0x12 */ {},
    /* 0x13 */ {},
    /* 0x14 */ {},
    /* 0x15 */ {},
    /* 0x16 */ {},
    /* 0x17 */ {},
    /* 0x18 */ {},
    /* 0x19 */ {},
    /* 0x1A */ {},
    /* 0x1B */ {},
    /* 0x1C */ {},
    /* 0x1D */ {},
    /* 0x1E */ {},
    /* 0x1F */ {},

    /* 0x20 */ {},
    /* 0x21 */ {},
    /* 0x22 */ {},
    /* 0x23 */ {},
    /* 0x24 */ {},
    /* 0x25 */ {},
    /* 0x26 */ {},
    /* 0x27 */ {},
    /* 0x28 */ {},
    /* 0x29 */ {},
    /* 0x2A */ {},
    /* 0x2B */ {},
    /* 0x2C */ {},
    /* 0x2D */ {},
    /* 0x2E */ {},
    /* 0x2F */ {},

    /* 0x30 */ {},
    /* 0x31 */ {},
    /* 0x32 */ {},
    /* 0x33 */ {},
    /* 0x34 */ {},
    /* 0x35 */ {},
    /* 0x36 */ {},
    /* 0x37 */ {},
    /* 0x38 */ {},
    /* 0x39 */ {},
    /* 0x3A */ {},
    /* 0x3B */ {},
    /* 0x3C */ {},
    /* 0x3D */ {},
    /* 0x3E */ {},
    /* 0x3F */ {},

    /* 0x40 */ {},
    /* 0x41 */ {},
    /* 0x42 */ {},
    /* 0x43 */ {},
    /* 0x44 */ {},
    /* 0x45 */ {},
    /* 0x46 */ {},
    /* 0x47 */ {},
    /* 0x48 */ {},
    /* 0x49 */ {},
    /* 0x4A */ {},
    /* 0x4B */ {},
    /* 0x4C */ {},
    /* 0x4D */ {},
    /* 0x4E */ {},
    /* 0x4F */ {},

    /* 0x50 */ {},
    /* 0x51 */ {},
    /* 0x52 */ {},
    /* 0x53 */ {},
    /* 0x54 */ {},
    /* 0x55 */ {},
    /* 0x56 */ {},
    /* 0x57 */ {},
    /* 0x58 */ {},
    /* 0x59 */ {},
    /* 0x5A */ {},
    /* 0x5B */ {},
    /* 0x5C */ {},
    /* 0x5D */ {},
    /* 0x5E */ {},
    /* 0x5F */ {},

    /* 0x60 */ {},
    /* 0x61 */ {},
    /* 0x62 */ {},
    /* 0x63 */ {},
    /* 0x64 */ {},
    /* 0x65 */ {},
    /* 0x66 */ {},
    /* 0x67 */ {},
    /* 0x68 */ {},
    /* 0x69 */ {},
    /* 0x6A */ {},
    /* 0x6B */ {},
    /* 0x6C */ {},
    /* 0x6D */ {},
    /* 0x6E */ {},
    /* 0x6F */ {},

    /* 0x70 */ {},
    /* 0x71 */ {},
    /* 0x72 */ {},
    /* 0x73 */ {},
    /* 0x74 */ {},
    /* 0x75 */ {},
    /* 0x76 */ {},
    /* 0x77 */ {},
    /* 0x78 */ {},
    /* 0x79 */ {},
    /* 0x7A */ {},
    /* 0x7B */ {},
    /* 0x7C */ {},
    /* 0x7D */ {},
    /* 0x7E */ {},
    /* 0x7F */ {},

    /* 0x80 */ {},
    /* 0x81 */ {},
    /* 0x82 */ {},
    /* 0x83 */ {},
    /* 0x84 */ {},
    /* 0x85 */ {},
    /* 0x86 */ {},
    /* 0x87 */ {},
    /* 0x88 */ {},
    /* 0x89 */ {},
    /* 0x8A */ {},
    /* 0x8B */ {},
    /* 0x8C */ {},
    /* 0x8D */ {},
    /* 0x8E */ {},
    /* 0x8F */ {},

    /* 0x90 */ {},
    /* 0x91 */ {},
    /* 0x92 */ {},
    /* 0x93 */ {},
    /* 0x94 */ {},
    /* 0x95 */ {},
    /* 0x96 */ {},
    /* 0x97 */ {},
    /* 0x98 */ {},
    /* 0x99 */ {},
    /* 0x9A */ {},
    /* 0x9B */ {},
    /* 0x9C */ {},
    /* 0x9D */ {},
    /* 0x9E */ {},
    /* 0x9F */ {},

    /* 0xA0 */ {},
    /* 0xA1 */ { &CPU::LDA, AddressMode::IndexedIndirect, 2, 6, 0 },
    /* 0xA2 */ { &CPU::LDX, AddressMode::Immediate, 2, 2, 0},
    /* 0xA3 */ {},
    /* 0xA4 */ {},
    /* 0xA5 */ { &CPU::LDA, AddressMode::ZeroPage, 2, 3, 0 },
    /* 0xA6 */ {},
    /* 0xA7 */ {},
    /* 0xA8 */ {},
    /* 0xA9 */ { &CPU::LDA, AddressMode::Immediate, 2, 2, 0 },
    /* 0xAA */ {},
    /* 0xAB */ {},
    /* 0xAC */ {},
    /* 0xAD */ { &CPU::LDA, AddressMode::Absolute, 3, 4, 0 },
    /* 0xAE */ {},
    /* 0xAF */ {},

    /* 0xB0 */ {},
    /* 0xB1 */ { &CPU::LDA, AddressMode::IndirectIndexed, 2, 5, 1 },
    /* 0xB2 */ {},
    /* 0xB3 */ {},
    /* 0xB4 */ {},
    /* 0xB5 */ { &CPU::LDA, AddressMode::ZeroPageX, 2, 4, 0 },
    /* 0xB6 */ {},
    /* 0xB7 */ {},
    /* 0xB8 */ {},
    /* 0xB9 */ { &CPU::LDA, AddressMode::AbsoluteY, 3, 4, 1 },
    /* 0xBA */ {},
    /* 0xBB */ {},
    /* 0xBC */ {},
    /* 0xBD */ { &CPU::LDA, AddressMode::AbsoluteX, 3, 4, 1 },
    /* 0xBE */ {},
    /* 0xBF */ {},

    /* 0xC0 */ {},
    /* 0xC1 */ {},
    /* 0xC2 */ {},
    /* 0xC3 */ {},
    /* 0xC4 */ {},
    /* 0xC5 */ {},
    /* 0xC6 */ {},
    /* 0xC7 */ {},
    /* 0xC8 */ {},
    /* 0xC9 */ {},
    /* 0xCA */ {},
    /* 0xCB */ {},
    /* 0xCC */ {},
    /* 0xCD */ {},
    /* 0xCE */ {},
    /* 0xCF */ {},

    /* 0xD0 */ {},
    /* 0xD1 */ {},
    /* 0xD2 */ {},
    /* 0xD3 */ {},
    /* 0xD4 */ {},
    /* 0xD5 */ {},
    /* 0xD6 */ {},
    /* 0xD7 */ {},
    /* 0xD8 */ {},
    /* 0xD9 */ {},
    /* 0xDA */ {},
    /* 0xDB */ {},
    /* 0xDC */ {},
    /* 0xDD */ {},
    /* 0xDE */ {},
    /* 0xDF */ {},

    /* 0xE0 */ {},
    /* 0xE1 */ {},
    /* 0xE2 */ {},
    /* 0xE3 */ {},
    /* 0xE4 */ {},
    /* 0xE5 */ {},
    /* 0xE6 */ {},
    /* 0xE7 */ {},
    /* 0xE8 */ {},
    /* 0xE9 */ {},
    /* 0xEA */ { &CPU::NOP, AddressMode::Implicit, 1, 2, 0 },
    /* 0xEB */ {},
    /* 0xEC */ {},
    /* 0xED */ {},
    /* 0xEE */ {},
    /* 0xEF */ {},

    /* 0xF0 */ {},
    /* 0xF1 */ {},
    /* 0xF2 */ {},
    /* 0xF3 */ {},
    /* 0xF4 */ {},
    /* 0xF5 */ {},
    /* 0xF6 */ {},
    /* 0xF7 */ {},
    /* 0xF8 */ {},
    /* 0xF9 */ {},
    /* 0xFA */ {},
    /* 0xFB */ {},
    /* 0xFC */ {},
    /* 0xFD */ {},
    /* 0xFE */ {},
    /* 0xFF */ {}
};

// Load/Store Operations

void CPU::LDA(Operand const& operand)
{
    a = memoryBus->Read(operand.address);
    s = SetZN(s, a);
}

void CPU::LDX(Operand const& operand)
{
    x = memoryBus->Read(operand.address);
}

void CPU::LDY(Operand const& operand)
{
    y = memoryBus->Read(operand.address);
}

void CPU::STA(Operand const& operand)
{
    memoryBus->Write(operand.address, a);
}

void CPU::STX(Operand const& operand)
{
    memoryBus->Write(operand.address, x);
}

void CPU::STY(Operand const& operand)
{
    memoryBus->Write(operand.address, y);
}

// Register Transfers

void CPU::TAX(Operand const&)
{
}

void CPU::TAY(Operand const&)
{
}

void CPU::TXA(Operand const&)
{
}

void CPU::TYA(Operand const&)
{
}

// Stack Operations

void CPU::TSX(Operand const&)
{
}

void CPU::TXS(Operand const&)
{
}

void CPU::PHA(Operand const&)
{
}

void CPU::PHP(Operand const&)
{ 
}

void CPU::PLA(Operand const&)
{
}

void CPU::PLP(Operand const&)
{
}

// Logical

void CPU::AND(Operand const&)
{
}

void CPU::EOR(Operand const&)
{
}

void CPU::ORA(Operand const&)
{
}

void CPU::BIT(Operand const&)
{
}

// Arithmetic

void CPU::ADC(Operand const&)
{
}

void CPU::SBC(Operand const&)
{
}

void CPU::CMP(Operand const&)
{
}

void CPU::CPX(Operand const&)
{
}

void CPU::CPY(Operand const&)
{
}

// Increments & Decrements

void CPU::INC(Operand const&)
{
}

void CPU::INX(Operand const&)
{
}

void CPU::INY(Operand const&)
{
}

void CPU::DEC(Operand const&)
{
}

void CPU::DEX(Operand const&)
{
}

void CPU::DEY(Operand const&)
{
}

// Shifts

void CPU::ASL(Operand const&)
{
}

void CPU::LSR(Operand const&)
{
}

void CPU::ROL(Operand const&)
{
}

void CPU::ROR(Operand const&)
{
}

// Jumps & Calls

void CPU::JMP(Operand const&)
{
}

void CPU::JSR(Operand const&)
{
}

void CPU::RTS(Operand const&)
{
}

// Branches

void CPU::BCC(Operand const&)
{
}

void CPU::BCS(Operand const&)
{
}

void CPU::BEQ(Operand const&)
{
}

void CPU::BMI(Operand const&)
{
}

void CPU::BNE(Operand const&)
{
}

void CPU::BPL(Operand const&)
{
}

void CPU::BVC(Operand const&)
{
}

void CPU::BVS(Operand const&)
{
}

// Status Flag Changes

void CPU::CLC(Operand const&)
{
}

void CPU::CLD(Operand const&)
{
}

void CPU::CLI(Operand const&)
{
    
}

void CPU::CLV(Operand const&)
{
}

void CPU::SEC(Operand const&)
{
}

void CPU::SED(Operand const&)
{
}

void CPU::SEI(Operand const&)
{
}

// System Functions

void CPU::BRK(Operand const&)
{
}

void CPU::NOP(Operand const&)
{
}

void CPU::RTI(Operand const&)
{
}

} // nes
