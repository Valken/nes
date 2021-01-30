#include "cpu.h"
#include "memory.h"
#include <functional>
#include <cassert>

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
    
    return Operand { .value = address, .address = address, .addressMode = addressMode };
}

uint16_t CPU::Read16(uint16_t address) const
{
    uint16_t lo = memoryBus->Read(address);
    uint16_t hi = memoryBus->Read(address + 1);

    return (hi << 8) | lo;
}

InstructionInfo CPU::InstructionInfo[256] =
{
    /* 0x00 */ { &CPU::BRK, AddressMode::Implicit, 1, 7, 0 },
    /* 0x01 */ { &CPU::ORA, AddressMode::IndexedIndirect, 2, 6, 0 },
    /* 0x02 */ {},
    /* 0x03 */ {},
    /* 0x04 */ {},
    /* 0x05 */ { &CPU::ORA, AddressMode::ZeroPage, 2, 3, 0 },
    /* 0x06 */ { &CPU::ASL, AddressMode::ZeroPage, 2, 5, 0 },
    /* 0x07 */ {},
    /* 0x08 */ { &CPU::PHP, AddressMode::Implicit, 1, 3, 0 },
    /* 0x09 */ { &CPU::ORA, AddressMode::Immediate, 2, 2, 0 },
    /* 0x0A */ { &CPU::ASL, AddressMode::Accumulator, 1, 2, 0 },
    /* 0x0B */ {},
    /* 0x0C */ {},
    /* 0x0D */ { &CPU::ORA, AddressMode::Absolute, 3, 4, 0 },
    /* 0x0E */ { &CPU::ASL, AddressMode::Absolute, 3, 6, 0 },
    /* 0x0F */ {},
    
    /* 0x10 */ { &CPU::BPL, AddressMode::Relative, 2, 2, 0 },
    /* 0x11 */ { &CPU::ORA, AddressMode::IndirectIndexed, 2, 5, 1 },
    /* 0x12 */ {},
    /* 0x13 */ {},
    /* 0x14 */ {},
    /* 0x15 */ { &CPU::ORA, AddressMode::ZeroPageX, 2, 4, 0 },
    /* 0x16 */ { &CPU::ASL, AddressMode::ZeroPageX, 2, 6, 0 },
    /* 0x17 */ {},
    /* 0x18 */ { &CPU::CLC, AddressMode::Implicit, 1, 2, 0 },
    /* 0x19 */ { &CPU::ORA, AddressMode::AbsoluteY, 3, 4, 1 },
    /* 0x1A */ {},
    /* 0x1B */ {},
    /* 0x1C */ {},
    /* 0x1D */ { &CPU::ORA, AddressMode::AbsoluteX, 2, 4, 1 },
    /* 0x1E */ { &CPU::ASL, AddressMode::AbsoluteX, 3, 7, 0 },
    /* 0x1F */ {},
    
    /* 0x20 */ { &CPU::JSR, AddressMode::Absolute, 3, 6, 0 },
    /* 0x21 */ { &CPU::AND, AddressMode::IndirectIndexed, 2, 6, 0 },
    /* 0x22 */ {},
    /* 0x23 */ {},
    /* 0x24 */ { &CPU::BIT, AddressMode::ZeroPage, 2, 3 },
    /* 0x25 */ { &CPU::AND, AddressMode::ZeroPage, 2, 3 },
    /* 0x26 */ { &CPU::ROL, AddressMode::ZeroPage, 2, 5, 0 },
    /* 0x27 */ {},
    /* 0x28 */ { &CPU::PLP, AddressMode::Implicit, 1, 4, 0 },
    /* 0x29 */ { &CPU::AND, AddressMode::Immediate, 2, 2, 0 },
    /* 0x2A */ { &CPU::ROL, AddressMode::Accumulator, 1, 2, 0 },
    /* 0x2B */ {},
    /* 0x2C */ { &CPU::BIT, AddressMode::Absolute, 3, 4, 0 },
    /* 0x2D*/ { &CPU::AND, AddressMode::Absolute, 3, 4, 0 },
    /* 0x2E */ { &CPU::ROL, AddressMode::Absolute, 3, 6, 0 },
    /* 0x2F */ {},
    
    /* 0x30 */ { &CPU::BMI, AddressMode::Relative, 2, 2, 0 },
    /* 0x31 */ { &CPU::AND, AddressMode::IndexedIndirect, 2, 5, 1 },
    /* 0x32 */ {},
    /* 0x33 */ {},
    /* 0x34 */ {},
    /* 0x35 */ { &CPU::AND, AddressMode::ZeroPageX, 2, 4, 0 },
    /* 0x36 */ { &CPU::ROL, AddressMode::ZeroPageX, 2, 6, 0 },
    /* 0x37 */ {},
    /* 0x38 */ { &CPU::SEC, AddressMode::Implicit, 1, 2, 0 },
    /* 0x39 */ { &CPU::AND, AddressMode::AbsoluteY, 3, 4, 1 },
    /* 0x3A */ {},
    /* 0x3B */ {},
    /* 0x3C */ {},
    /* 0x3D */ { &CPU::AND, AddressMode::AbsoluteX, 3, 4, 1 },
    /* 0x3E */ { &CPU::ROL, AddressMode::AbsoluteX, 3, 7, 0 },
    /* 0x3F */ {},
    
    /* 0x40 */ { &CPU::RTI, AddressMode::Implicit, 1, 6, 0 },
    /* 0x41 */ { &CPU::EOR, AddressMode::IndexedIndirect, 2, 6 },
    /* 0x42 */ {},
    /* 0x43 */ {},
    /* 0x44 */ {},
    /* 0x45 */ { &CPU::EOR, AddressMode::ZeroPage, 2, 3, 0 },
    /* 0x46 */ { &CPU::LSR, AddressMode::ZeroPage, 2, 5, 0 },
    /* 0x47 */ {},
    /* 0x48 */ { &CPU::PHA, AddressMode::Implicit, 1, 3, 0 },
    /* 0x49 */ { &CPU::EOR, AddressMode::Immediate, 2, 2, 0 },
    /* 0x4A */ { &CPU::LSR, AddressMode::Accumulator, 1, 2, 0 },
    /* 0x4B */ {},
    /* 0x4C */ { &CPU::JMP, AddressMode::Absolute, 3, 3, 0 },
    /* 0x4D*/ { &CPU::EOR, AddressMode::Absolute, 3, 4, 1 },
    /* 0x4E */ { &CPU::LSR, AddressMode::Absolute, 3, 6, 0 },
    /* 0x4F */ {},
    
    /* 0x50 */ { &CPU::BVC, AddressMode::Relative, 2, 2, 0 },
    /* 0x51 */ { &CPU::EOR, AddressMode::IndirectIndexed, 2, 5, 1 },
    /* 0x52 */ {},
    /* 0x53 */ {},
    /* 0x54 */ {},
    /* 0x55 */ { &CPU::EOR, AddressMode::ZeroPageX, 2, 4 },
    /* 0x56 */ { &CPU::LSR, AddressMode::ZeroPageX, 2, 6, 0 },
    /* 0x57 */ {},
    /* 0x58 */ { &CPU::CLI, AddressMode::Implicit, 1, 2, 0 },
    /* 0x59 */ { &CPU::EOR, AddressMode::AbsoluteY, 3, 4, 1 },
    /* 0x5A */ {},
    /* 0x5B */ {},
    /* 0x5C */ {},
    /* 0x5D */ { &CPU::EOR, AddressMode::AbsoluteX, 3, 4, 1 },
    /* 0x5E */ { &CPU::LSR, AddressMode::AbsoluteX, 3, 7, 0 },
    /* 0x5F */ {},
    
    /* 0x60 */ { &CPU::RTS, AddressMode::Implicit, 1, 6, 0 },
    /* 0x61 */ { &CPU::ADC, AddressMode::IndexedIndirect, 2, 6, 0 },
    /* 0x62 */ {},
    /* 0x63 */ {},
    /* 0x64 */ {},
    /* 0x65 */ { &CPU::ADC, AddressMode::ZeroPage, 2, 3 },
    /* 0x66 */ { &CPU::ROR, AddressMode::ZeroPage, 2, 5, 0 },
    /* 0x67 */ {},
    /* 0x68 */ { &CPU::PLA, AddressMode::Implicit, 1, 4, 0 },
    /* 0x69 */ { &CPU::ADC, AddressMode::Immediate, 2, 2, 0 },
    /* 0x6A */ { &CPU::ROR, AddressMode::Accumulator, 1, 2, 0 },
    /* 0x6B */ {},
    /* 0x6C */ { &CPU::JMP, AddressMode::Indirect, 3, 5, 0 },
    /* 0x6D */ { &CPU::ADC, AddressMode::Absolute, 3, 4, 0 },
    /* 0x6E */ { &CPU::ROR, AddressMode::Absolute, 3, 6, 0 },
    /* 0x6F */ {},
    
    /* 0x70 */ { &CPU::BVS, AddressMode::Relative, 2, 2, 0 },
    /* 0x71 */ { &CPU::ADC, AddressMode::IndirectIndexed, 2, 5, 1 },
    /* 0x72 */ {},
    /* 0x73 */ {},
    /* 0x74 */ {},
    /* 0x75 */ { &CPU::ADC, AddressMode::ZeroPageX, 2, 4, 0 },
    /* 0x76 */ { &CPU::ROR, AddressMode::ZeroPageX, 2, 6, 0 },
    /* 0x77 */ {},
    /* 0x78 */ { &CPU::SEI, AddressMode::Implicit, 1, 2, 0 },
    /* 0x79 */ { &CPU::ADC, AddressMode::AbsoluteY, 3, 4, 1 },
    /* 0x7A */ {},
    /* 0x7B */ {},
    /* 0x7C */ {},
    /* 0x7D*/ { &CPU::ADC, AddressMode::AbsoluteX, 3, 4, 1 },
    /* 0x7E */ { &CPU::ROR, AddressMode::AbsoluteX, 3, 7, 0 },
    /* 0x7F */ {},
    
    /* 0x80 */ {},
    /* 0x81 */ { &CPU::STA, AddressMode::IndexedIndirect, 2, 6, 0 },
    /* 0x82 */ {},
    /* 0x83 */ {},
    /* 0x84 */ { &CPU::STY, AddressMode::ZeroPage, 2, 3, 0 },
    /* 0x85 */ { &CPU::STA, AddressMode::ZeroPage, 2, 3, 0 },
    /* 0x86 */ { &CPU::STX, AddressMode::ZeroPage, 2, 3, 0 },
    /* 0x87 */ {},
    /* 0x88 */ { &CPU::DEY, AddressMode::Implicit, 1, 2, 0 },
    /* 0x89 */ {},
    /* 0x8A */ { &CPU::TXA, AddressMode::Immediate, 1, 2, 0 },
    /* 0x8B */ {},
    /* 0x8C */ { &CPU::STY, AddressMode::Absolute, 3, 4, 0 },
    /* 0x8D */ { &CPU::STA, AddressMode::Absolute, 3, 4, 0 },
    /* 0x8E */ { &CPU::STX, AddressMode::Absolute, 3, 4, 0 },
    /* 0x8F */ {},
    
    /* 0x90 */ { &CPU::BCC, AddressMode::Relative, 2, 2, 0 },
    /* 0x91 */ { &CPU::STA, AddressMode::IndirectIndexed, 2, 6, 0 },
    /* 0x92 */ {},
    /* 0x93 */ {},
    /* 0x94 */ { &CPU::STY, AddressMode::ZeroPageX, 2, 4, 0 },
    /* 0x95 */ { &CPU::STA, AddressMode::ZeroPageX, 2, 4, 0 },
    /* 0x96 */ { &CPU::STX, AddressMode::ZeroPageY, 2, 4, 0 },
    /* 0x97 */ {},
    /* 0x98 */ { &CPU::TYA, AddressMode::Immediate, 1, 2, 0 },
    /* 0x99 */ { &CPU::STA, AddressMode::AbsoluteY, 3, 5, 0 },
    /* 0x9A */ { &CPU::TSX, AddressMode::Immediate, 1, 2, 0 },
    /* 0x9B */ {},
    /* 0x9C */ {},
    /* 0x9D */ { &CPU::STA, AddressMode::AbsoluteX, 3, 5, 0 },
    /* 0x9E */ {},
    /* 0x9F */ {},
    
    /* 0xA0 */ { &CPU::LDY, AddressMode::Immediate, 2, 2, 0 },
    /* 0xA1 */ { &CPU::LDA, AddressMode::Immediate, 2, 2, 0 },
    /* 0xA2 */ { &CPU::LDX, AddressMode::Immediate, 2, 2, 0 },
    /* 0xA3 */ {},
    /* 0xA4 */ { &CPU::LDY, AddressMode::ZeroPage, 2, 3, 0 },
    /* 0xA5 */ { &CPU::LDA, AddressMode::ZeroPage, 2, 3, 0 },
    /* 0xA6 */ { &CPU::LDX, AddressMode::ZeroPage, 2, 3, 0 },
    /* 0xA7 */ {},
    /* 0xA8 */ { &CPU::TAY, AddressMode::Implicit, 1, 2, 0 },
    /* 0xA9 */ { &CPU::LDA, AddressMode::Immediate, 2, 2, 0 },
    /* 0xAA */ { &CPU::TAX, AddressMode::Implicit, 1, 2, 0 },
    /* 0xAB */ {},
    /* 0xAC */ { &CPU::LDY, AddressMode::Absolute, 3, 4, 0 },
    /* 0xAD */ { &CPU::LDA, AddressMode::Absolute, 3, 4, 0 },
    /* 0xAE */ { &CPU::LDX, AddressMode::Absolute, 3, 4, 0 },
    /* 0xAF */ {},
    
    /* 0xB0 */ { &CPU::BCS, AddressMode::Relative, 2, 2, 0 },
    /* 0xB1 */ {},
    /* 0xB2 */ {},
    /* 0xB3 */ {},
    /* 0xB4 */ { &CPU::LDY, AddressMode::ZeroPageX, 2, 4, 0 },
    /* 0xB5 */ { &CPU::LDA, AddressMode::ZeroPageX, 2, 4, 0 },
    /* 0xB6 */ { &CPU::LDX, AddressMode::ZeroPageY, 2, 4, 0 },
    /* 0xB7 */ {},
    /* 0xB8 */ { &CPU::CLV, AddressMode::Implicit, 1, 2, 0 },
    /* 0xB9 */ { &CPU::LDA, AddressMode::AbsoluteY, 3, 4, 1 },
    /* 0xBA */ { &CPU::TSX, AddressMode::Immediate, 1, 2, 0 },
    /* 0xBB */ {},
    /* 0xBC */ { &CPU::LDY, AddressMode::AbsoluteX, 3, 4, 1 },
    /* 0xBD */ { &CPU::LDA, AddressMode::AbsoluteX, 3, 4, 1 },
    /* 0xBE */ { &CPU::LDX, AddressMode::AbsoluteY, 3, 4, 1 },
    /* 0xBF */ {},
    
    /* 0xC0 */ { &CPU::CPY, AddressMode::Immediate, 2, 2, 0 },
    /* 0xC1 */ { &CPU::CMP, AddressMode::IndexedIndirect, 2, 6, 0 },
    /* 0xC2 */ {},
    /* 0xC3 */ {},
    /* 0xC4 */ { &CPU::CPY, AddressMode::ZeroPage, 2, 3, 0 },
    /* 0xC5 */ { &CPU::CMP, AddressMode::ZeroPage, 2, 3, 0 },
    /* 0xC6 */ { &CPU::DEC, AddressMode::ZeroPage, 2, 5, 0 },
    /* 0xC7 */ {},
    /* 0xC8 */ { &CPU::INY, AddressMode::Implicit, 1, 2, 0 },
    /* 0xC9 */ { &CPU::CMP, AddressMode::Immediate, 2, 2, 0 },
    /* 0xCA */ { &CPU::DEX, AddressMode::Implicit, 1, 2, 0 },
    /* 0xCB */ {},
    /* 0xCC */ { &CPU::CPY, AddressMode::Absolute, 3, 4, 0 },
    /* 0xCD */ { &CPU::CMP, AddressMode::Absolute, 3, 4, 0 },
    /* 0xCE */ { &CPU::DEC, AddressMode::Absolute, 3, 6, 0 },
    /* 0xCF */ {},
    
    /* 0xD0 */ { &CPU::BNE, AddressMode::Relative, 2, 2, 0 },
    /* 0xD1 */ { &CPU::CMP, AddressMode::IndirectIndexed, 2, 5, 1 },
    /* 0xD2 */ {},
    /* 0xD3 */ {},
    /* 0xD4 */ {},
    /* 0xD5 */ { &CPU::CMP, AddressMode::ZeroPageX, 2, 4, 0 },
    /* 0xD6 */ { &CPU::DEC, AddressMode::ZeroPageX, 2, 6, 0 },
    /* 0xD7 */ {},
    /* 0xD8 */ { &CPU::CLD, AddressMode::Implicit, 1, 2, 0 },
    /* 0xD9 */ { &CPU::CMP, AddressMode::AbsoluteY, 3, 4, 1 },
    /* 0xDA */ {},
    /* 0xDB */ {},
    /* 0xDC */ {},
    /* 0xDD */ { &CPU::CMP, AddressMode::AbsoluteX, 3, 4, 1 },
    /* 0xDE */ { &CPU::DEC, AddressMode::AbsoluteX, 3, 7, 0 },
    /* 0xDF */ {},
    
    /* 0xE0 */ { &CPU::CPX, AddressMode::Immediate, 2, 2, 0 },
    /* 0xE1 */ { &CPU::SBC, AddressMode::IndexedIndirect },
    /* 0xE2 */ {},
    /* 0xE3 */ {},
    /* 0xE4 */ { &CPU::CPX, AddressMode::ZeroPage, 2, 3, 0 },
    /* 0xE5 */ { &CPU::SBC, AddressMode::ZeroPage, 2, 3, 0 },
    /* 0xE6 */ { &CPU::INC, AddressMode::ZeroPage, 2, 5, 0 },
    /* 0xE7 */ {},
    /* 0xE8 */ { &CPU::INX, AddressMode::Implicit, 1, 2, 0 },
    /* 0xE9 */ { &CPU::SBC, AddressMode::Immediate, 2, 2, 0 },
    /* 0xEA */ { &CPU::NOP, AddressMode::Implicit, 1, 2, 0},
    /* 0xEB */ {},
    /* 0xEC */ { &CPU::CPX, AddressMode::Absolute, 3, 4, 0 },
    /* 0xED */ { &CPU::SBC, AddressMode::Absolute, 3, 4, 0 },
    /* 0xEE */ { &CPU::INC, AddressMode::Absolute, 3, 6, 0 },
    /* 0xEF */ {},
    
    /* 0xF0 */ { &CPU::BEQ, AddressMode::Relative, 2, 2, 0 },
    /* 0xF1 */ { &CPU::SBC, AddressMode::IndirectIndexed, 2, 5, 1 },
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
    /* 0xFF */ {},
};


// Load/Store Operations

void CPU::LDA(Operand const& operand)
{
    a = memoryBus->Read(operand.value);
}

void CPU::LDX(Operand const&)
{
}

void CPU::LDY(Operand const&)
{
}

void CPU::STA(Operand const&)
{
}

void CPU::STX(Operand const&)
{
}

void CPU::STY(Operand const&)
{
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
