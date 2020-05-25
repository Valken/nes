#include <functional>
#include "cpu.h"
#include "memory.h"

namespace nes
{

CPU::CPU(Memory* const memoryBus) : pc(0x0000), a(0), x(0), y(0), p(0), s(0xFD), memoryBus(memoryBus)
{
}

void CPU::Step()
{
    // Interrupt?

    auto programCounter = pc;
    auto instruction = Fetch();
    auto instructionInfo = InstructionInfo[instruction];
    pc += instructionInfo.instructionSize;
    
    auto operand = Decode(instructionInfo.addressMode);
    
    std::invoke(instructionInfo.instruction, this, operand);

    // Return num cycles
}

uint8_t CPU::Fetch()
{
    return memoryBus->Read(pc);
}

Operand CPU::Decode(AddressMode addressMode) const
{
    switch (addressMode)
    {
        case AddressMode::Implicit:
        case AddressMode::Accumulator:
        case AddressMode::Immediate:
        case AddressMode::ZeroPage:
        case AddressMode::ZeroPageX:
        case AddressMode::ZeroPageY:
        case AddressMode::Relative:
        case AddressMode::Absolute:
        case AddressMode::AbsoluteX:
        case AddressMode::AbsoluteY:
        case AddressMode::Indirect:
        case AddressMode::IndexedIndirect:
        case AddressMode::IndirectIndexed:
            break;
    }
    return Operand { .value = 0x0, .address = 0x0, .addressMode = AddressMode::Indirect };
}

InstructionInfo CPU::InstructionInfo[256] =
{
    /* 0x00 */ { &CPU::BRK, },
    /* 0x01 */ { &CPU::ORA, },
    /* 0x02 */ {},
    /* 0x03 */ {},
    /* 0x04 */ {},
    /* 0x05 */ { &CPU::ORA, },
    /* 0x06 */ { &CPU::ASL, },
    /* 0x07 */ {},
    /* 0x08 */ { &CPU::PHP, },
    /* 0x09 */ { &CPU::ORA, },
    /* 0x0A */ { &CPU::ASL, },
    /* 0x0B */ {},
    /* 0x0C */ {},
    /* 0x0D*/ { &CPU::ORA, },
    /* 0x0E */ { &CPU::ASL, },
    /* 0x0F */ {},
    
    /* 0x10 */ { &CPU::BPL, },
    /* 0x11 */ { &CPU::ORA, },
    /* 0x12 */ {},
    /* 0x13 */ {},
    /* 0x14 */ {},
    /* 0x15 */ { &CPU::ORA, },
    /* 0x16 */ { &CPU::ASL, },
    /* 0x17 */ {},
    /* 0x18 */ { &CPU::CLC, },
    /* 0x19 */ { &CPU::ORA, },
    /* 0x1A */ {},
    /* 0x1B */ {},
    /* 0x1C */ {},
    /* 0x1D*/ { &CPU::ORA, },
    /* 0x1E */ { &CPU::ASL, },
    /* 0x1F */ {},
    
    /* 0x20 */ { &CPU::JSR, },
    /* 0x21 */ { &CPU::AND, },
    /* 0x22 */ {},
    /* 0x23 */ {},
    /* 0x24 */ { &CPU::BIT, },
    /* 0x25 */ { &CPU::AND, },
    /* 0x26 */ { &CPU::ROL, },
    /* 0x27 */ {},
    /* 0x28 */ { &CPU::PLP, },
    /* 0x29 */ { &CPU::AND, },
    /* 0x2A */ { &CPU::ROL, },
    /* 0x2B */ {},
    /* 0x2C */ { &CPU::BIT, },
    /* 0x2D*/ { &CPU::AND, },
    /* 0x2E */ { &CPU::ROL, },
    /* 0x2F */ {},
    
    /* 0x30 */ { &CPU::BMI, },
    /* 0x31 */ { &CPU::AND, },
    /* 0x32 */ {},
    /* 0x33 */ {},
    /* 0x34 */ {},
    /* 0x35 */ { &CPU::AND, },
    /* 0x36 */ { &CPU::ROL, },
    /* 0x37 */ {},
    /* 0x38 */ { &CPU::SEC, },
    /* 0x39 */ { &CPU::AND, },
    /* 0x3A */ {},
    /* 0x3B */ {},
    /* 0x3C */ {},
    /* 0x3D*/ { &CPU::AND, },
    /* 0x3E */ { &CPU::ROL, },
    /* 0x3F */ {},
    
    /* 0x40 */ { &CPU::RTI, },
    /* 0x41 */ { &CPU::EOR, },
    /* 0x42 */ {},
    /* 0x43 */ {},
    /* 0x44 */ {},
    /* 0x45 */ { &CPU::EOR, },
    /* 0x46 */ { &CPU::LSR, },
    /* 0x47 */ {},
    /* 0x48 */ { &CPU::PHA, },
    /* 0x49 */ { &CPU::EOR, },
    /* 0x4A */ { &CPU::LSR, },
    /* 0x4B */ {},
    /* 0x4C */ { &CPU::JMP, },
    /* 0x4D*/ { &CPU::EOR, },
    /* 0x4E */ { &CPU::LSR, },
    /* 0x4F */ {},
    
    /* 0x50 */ { &CPU::BVC, },
    /* 0x51 */ { &CPU::EOR, },
    /* 0x52 */ {},
    /* 0x53 */ {},
    /* 0x54 */ {},
    /* 0x55 */ { &CPU::EOR, },
    /* 0x56 */ { &CPU::LSR, },
    /* 0x57 */ {},
    /* 0x58 */ { &CPU::CLI },
    /* 0x59 */ { &CPU::EOR, },
    /* 0x5A */ {},
    /* 0x5B */ {},
    /* 0x5C */ {},
    /* 0x5D*/ { &CPU::EOR, },
    /* 0x5E */ { &CPU::LSR, },
    /* 0x5F */ {},
    
    /* 0x60 */ { &CPU::RTS, },
    /* 0x61 */ { &CPU::ADC, },
    /* 0x62 */ {},
    /* 0x63 */ {},
    /* 0x64 */ {},
    /* 0x65 */ { &CPU::ADC, },
    /* 0x66 */ { &CPU::ROR, },
    /* 0x67 */ {},
    /* 0x68 */ { &CPU::PLA, },
    /* 0x69 */ { &CPU::ADC, },
    /* 0x6A */ { &CPU::ROR, },
    /* 0x6B */ {},
    /* 0x6C */ { &CPU::JMP, },
    /* 0x6D*/ { &CPU::ADC, },
    /* 0x6E */ { &CPU::ROR, },
    /* 0x6F */ {},
    
    /* 0x70 */ { &CPU::BVS, },
    /* 0x71 */ { &CPU::ADC, },
    /* 0x72 */ {},
    /* 0x73 */ {},
    /* 0x74 */ {},
    /* 0x75 */ { &CPU::ADC, },
    /* 0x76 */ { &CPU::ROR, },
    /* 0x77 */ {},
    /* 0x78 */ { &CPU::SEI, },
    /* 0x79 */ { &CPU::ADC, },
    /* 0x7A */ {},
    /* 0x7B */ {},
    /* 0x7C */ {},
    /* 0x7D*/ { &CPU::ADC, },
    /* 0x7E */ { &CPU::ROR, },
    /* 0x7F */ {},
    
    /* 0x80 */ {},
    /* 0x81 */ { &CPU::STA, AddressMode::IndexedIndirect, 2, 6, 0 },
    /* 0x82 */ {},
    /* 0x83 */ {},
    /* 0x84 */ { &CPU::STY, AddressMode::ZeroPage, 2, 3, 0 },
    /* 0x85 */ { &CPU::STA, AddressMode::ZeroPage, 2, 3, 0 },
    /* 0x86 */ { &CPU::STX, AddressMode::ZeroPage, 2, 3, 0 },
    /* 0x87 */ {},
    /* 0x88 */ { &CPU::DEY, },
    /* 0x89 */ {},
    /* 0x8A */ { &CPU::TXA, AddressMode::Immediate, 1, 2, 0 },
    /* 0x8B */ {},
    /* 0x8C */ { &CPU::STY, AddressMode::Absolute, 3, 4, 0 },
    /* 0x8D*/ { &CPU::STA, AddressMode::Absolute, 3, 4, 0 },
    /* 0x8E */ { &CPU::STX, AddressMode::Absolute, 3, 4, 0 },
    /* 0x8F */ {},
    
    /* 0x90 */ { &CPU::BCC, },
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
    /* 0x9D*/ { &CPU::STA, AddressMode::AbsoluteX, 3, 5, 0 },
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
    /* 0xAD*/ { &CPU::LDA, AddressMode::Absolute, 3, 4, 0 },
    /* 0xAE */ { &CPU::LDX, AddressMode::Absolute, 3, 4, 0 },
    /* 0xAF */ {},
    
    /* 0xB0 */ { &CPU::BCS, },
    /* 0xB1 */ {},
    /* 0xB2 */ {},
    /* 0xB3 */ {},
    /* 0xB4 */ { &CPU::LDY, AddressMode::ZeroPageX, 2, 4, 0 },
    /* 0xB5 */ { &CPU::LDA, AddressMode::ZeroPageX, 2, 4, 0 },
    /* 0xB6 */ { &CPU::LDX, AddressMode::ZeroPageY, 2, 4, 0 },
    /* 0xB7 */ {},
    /* 0xB8 */ { &CPU::CLV, },
    /* 0xB9 */ { &CPU::LDA, AddressMode::AbsoluteY, 3, 4, 1 },
    /* 0xBA */ { &CPU::TSX, AddressMode::Immediate, 1, 2, 0 },
    /* 0xBB */ {},
    /* 0xBC */ { &CPU::LDY, AddressMode::AbsoluteX, 3, 4, 1 },
    /* 0xBD*/ { &CPU::LDA, AddressMode::AbsoluteX, 3, 4, 1 },
    /* 0xBE */ { &CPU::LDX, AddressMode::AbsoluteY, 3, 4, 1 },
    /* 0xBF */ {},
    
    /* 0xC0 */ { &CPU::CPY, },
    /* 0xC1 */ { &CPU::CMP, },
    /* 0xC2 */ {},
    /* 0xC3 */ {},
    /* 0xC4 */ { &CPU::CPY, },
    /* 0xC5 */ { &CPU::CMP, },
    /* 0xC6 */ { &CPU::DEC, },
    /* 0xC7 */ {},
    /* 0xC8 */ { &CPU::INY, },
    /* 0xC9 */ { &CPU::CMP, },
    /* 0xCA */ { &CPU::DEX, },
    /* 0xCB */ {},
    /* 0xCC */ { &CPU::CPY, },
    /* 0xCD*/ { &CPU::CMP, },
    /* 0xCE */ { &CPU::DEC, },
    /* 0xCF */ {},
    
    /* 0xD0 */ { &CPU::BNE, },
    /* 0xD1 */ { &CPU::CMP, },
    /* 0xD2 */ {},
    /* 0xD3 */ {},
    /* 0xD4 */ {},
    /* 0xD5 */ { &CPU::CMP, },
    /* 0xD6 */ { &CPU::DEC, },
    /* 0xD7 */ {},
    /* 0xD8 */ { &CPU::CLD, },
    /* 0xD9 */ { &CPU::CMP, },
    /* 0xDA */ {},
    /* 0xDB */ {},
    /* 0xDC */ {},
    /* 0xDD*/ { &CPU::CMP, },
    /* 0xDE */ { &CPU::DEC, },
    /* 0xDF */ {},
    
    /* 0xE0 */ { &CPU::CPX, },
    /* 0xE1 */ { &CPU::SBC, },
    /* 0xE2 */ {},
    /* 0xE3 */ {},
    /* 0xE4 */ { &CPU::CPX, },
    /* 0xE5 */ { &CPU::SBC, },
    /* 0xE6 */ { &CPU::INC, },
    /* 0xE7 */ {},
    /* 0xE8 */ { &CPU::INX, },
    /* 0xE9 */ { &CPU::SBC, },
    /* 0xEA */ { &CPU::NOP, AddressMode::Implicit, 1, 2, 0},
    /* 0xEB */ {},
    /* 0xEC */ { &CPU::CPX, },
    /* 0xED*/ { &CPU::SBC, },
    /* 0xEE */ { &CPU::INC, },
    /* 0xEF */ {},
    
    /* 0xF0 */ { &CPU::BEQ, },
    /* 0xF1 */ { &CPU::SBC, },
    /* 0xF2 */ {},
    /* 0xF3 */ {},
    /* 0xF4 */ {},
    /* 0xF5 */ { &CPU::SBC, },
    /* 0xF6 */ { &CPU::INC, },
    /* 0xF7 */ {},
    /* 0xF8 */ { &CPU::SED, },
    /* 0xF9 */ { &CPU::SBC, },
    /* 0xFA */ {},
    /* 0xFB */ {},
    /* 0xFC */ {},
    /* 0xFD*/ { &CPU::SBC, },
    /* 0xFE */ { &CPU::INC, },
    /* 0xFF */ {},
};


// Load/Store Operations

void CPU::LDA(Operand const&)
{
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
