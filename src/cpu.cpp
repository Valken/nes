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

static uint8_t SetFlag(uint8_t flags, uint8_t value, bool enabled)
{
    if (enabled)
        return flags |= value;
    else
        return flags &= ~value;
}

static uint8_t SetZN(uint8_t flags, uint8_t value)
{
    flags = SetFlag(flags, Z, value == 0);
    flags = SetFlag(flags, N, value & 0x80);
    return  flags;
}

static bool AddressPagesDifferent(uint16_t a, uint16_t b)
{
    // Compare high bits of two address.If they're different,
    // we've crossed another to another page of memory which
    // is an additional cycle
    return (a & 0xFF00) != (b & 0xFF00);
}

namespace nes
{

CPU::CPU(Memory* const memoryBus) : pc(0x0000), a(0), x(0), y(0), s(0x00), memoryBus(memoryBus)
{
}

uint8_t CPU::Step()
{
    // Interrupt?

    auto instruction = Fetch();
    auto instructionInfo = InstructionInfo[instruction];
    auto operand = Decode(instructionInfo.addressMode);
    pc += instructionInfo.instructionSize;
    std::invoke(instructionInfo.instruction, this, operand);

    return instructionInfo.cycles + (operand.pageCrossed ? instructionInfo.pageCycles : 0);
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

// Some docs on addressing modes
// http://www.obelisk.me.uk/6502/addressing.html
// http://www.emulator101.com/6502-addressing-modes.html
// http://wiki.nesdev.com/w/index.php/CPU_addressing_modes
Operand CPU::Decode(AddressMode addressMode) const
{
    uint16_t address = 0x00;
    bool pageCrossed = false;

    switch (addressMode)
    {
        case AddressMode::Implicit:
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
            assert(false);
            break;

        case AddressMode::Absolute:
            address = Read16(pc + 1);
            break;

        case AddressMode::AbsoluteX:
            address = Read16(pc + 1) + x;
            pageCrossed = AddressPagesDifferent(address - x, address);
            break;

        case AddressMode::AbsoluteY:
            address = Read16(pc + 1) + y;
            pageCrossed = AddressPagesDifferent(address - y, address);
            break;

        case AddressMode::Indirect:
            address = ReadBugged(Read16(pc + 1));
            break;

        case AddressMode::IndexedIndirect:
            {
                uint16_t arg = memoryBus->Read(pc + 1);
                address = memoryBus->Read((arg + x + 1) & 0xFF) << 8 | memoryBus->Read((arg + x) & 0xFF);
            }
            break;

        case AddressMode::IndirectIndexed:
            {
                uint16_t arg = memoryBus->Read(pc + 1);
                uint16_t low = memoryBus->Read(arg & 0xFF);
                uint16_t high = memoryBus->Read((arg + 1) & 0xFF);
                address = (high << 8 | low) + y;
                pageCrossed = AddressPagesDifferent(address - y, address);
            }
            break;
    }
    
    return Operand { .address = address, .addressMode = addressMode, .pageCrossed = pageCrossed };
}

uint16_t CPU::Read16(uint16_t address) const
{
    uint8_t low = memoryBus->Read(address);
    uint8_t high = memoryBus->Read(address + 1);

    return (high << 8) | low;
}

uint16_t CPU::ReadBugged(uint16_t address) const
{
    // Used for indirect addressing. If lsb of value is on page boundary then
    // the the msb wraps around to the start of the page again.
    uint8_t low = memoryBus->Read(address);
    uint8_t high = memoryBus->Read((address & 0xFF00) | ((address + 1) & 0x00FF));

    return high << 8 | low;
}

void CPU::Push(uint8_t value)
{
    uint16_t address = 0x100 | sp;
    memoryBus->Write(address, value);
    sp--;
}

uint8_t CPU::Pop()
{
    sp++;
    uint16_t address = 0x100 | sp;
    return memoryBus->Read(address);
}

InstructionInfo CPU::InstructionInfo[256] =
{
    /* 0x00 */ {},
    /* 0x01 */ {},
    /* 0x02 */ {},
    /* 0x03 */ {},
    /* 0x04 */ {},
    /* 0x05 */ {},
    /* 0x06 */ { &CPU::ASL, AddressMode::ZeroPage, 2, 5, 0 },
    /* 0x07 */ {},
    /* 0x08 */ { &CPU::PHP, AddressMode::Implicit, 1, 3, 0 },
    /* 0x09 */ {},
    /* 0x0A */ { &CPU::ASL, AddressMode::Accumulator, 1, 2, 0 },
    /* 0x0B */ {},
    /* 0x0C */ {},
    /* 0x0D */ {},
    /* 0x0E */ { &CPU::ASL, AddressMode::Absolute, 3, 6, 0 },
    /* 0x0F */ {},

    /* 0x10 */ {},
    /* 0x11 */ {},
    /* 0x12 */ {},
    /* 0x13 */ {},
    /* 0x14 */ {},
    /* 0x15 */ {},
    /* 0x16 */ { &CPU::ASL, AddressMode::ZeroPageX, 2, 6, 0 },
    /* 0x17 */ {},
    /* 0x18 */ { &CPU::CLC, AddressMode::Immediate, 1, 2, 0 },
    /* 0x19 */ {},
    /* 0x1A */ {},
    /* 0x1B */ {},
    /* 0x1C */ {},
    /* 0x1D */ {},
    /* 0x1E */ { &CPU::ASL, AddressMode::AbsoluteX, 3, 7, 0 },
    /* 0x1F */ {},

    /* 0x20 */ {},
    /* 0x21 */ {},
    /* 0x22 */ {},
    /* 0x23 */ {},
    /* 0x24 */ {},
    /* 0x25 */ {},
    /* 0x26 */ { &CPU::ROL, AddressMode::ZeroPage, 2, 5, 0 },
    /* 0x27 */ {},
    /* 0x28 */ { &CPU::PLP, AddressMode::Implicit, 1, 4, 0 },
    /* 0x29 */ {},
    /* 0x2A */ { &CPU::ROL, AddressMode::Accumulator, 1, 2, 0 },
    /* 0x2B */ {},
    /* 0x2C */ {},
    /* 0x2D */ {},
    /* 0x2E */ { &CPU::ROL, AddressMode::Absolute, 2, 6, 0 },
    /* 0x2F */ {},

    /* 0x30 */ {},
    /* 0x31 */ {},
    /* 0x32 */ {},
    /* 0x33 */ {},
    /* 0x34 */ {},
    /* 0x35 */ {},
    /* 0x36 */ { &CPU::ROL, AddressMode::ZeroPageX, 2, 6, 0 },
    /* 0x37 */ {},
    /* 0x38 */ { &CPU::SEC, AddressMode::Implicit, 1, 2, 0 },
    /* 0x39 */ {},
    /* 0x3A */ {},
    /* 0x3B */ {},
    /* 0x3C */ {},
    /* 0x3D */ {},
    /* 0x3E */ { &CPU::ROL, AddressMode::AbsoluteX, 2, 7, 0 },
    /* 0x3F */ {},

    /* 0x40 */ {},
    /* 0x41 */ {},
    /* 0x42 */ {},
    /* 0x43 */ {},
    /* 0x44 */ {},
    /* 0x45 */ {},
    /* 0x46 */ {},
    /* 0x47 */ {},
    /* 0x48 */ { &CPU::PHA, AddressMode::Implicit, 1, 3, 0 },
    /* 0x49 */ {},
    /* 0x4A */ {},
    /* 0x4B */ {},
    /* 0x4C */ { &CPU::JMP, AddressMode::Absolute, 3, 3, 0 },
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
    /* 0x61 */ { &CPU::ADC, AddressMode::IndexedIndirect, 2, 6, 0 },
    /* 0x62 */ {},
    /* 0x63 */ {},
    /* 0x64 */ {},
    /* 0x65 */ { &CPU::ADC, AddressMode::ZeroPage, 2, 3, 0 },
    /* 0x66 */ {},
    /* 0x67 */ {},
    /* 0x68 */ { &CPU::PLA, AddressMode::Implicit, 1, 4, 0 },
    /* 0x69 */ { &CPU::ADC, AddressMode::Immediate, 2, 2, 0 },
    /* 0x6A */ {},
    /* 0x6B */ {},
    /* 0x6C */ { &CPU::JMP, AddressMode::Indirect, 3, 5, 0 },
    /* 0x6D */ { &CPU::ADC, AddressMode::Absolute, 3, 4 ,0 },
    /* 0x6E */ {},
    /* 0x6F */ {},

    /* 0x70 */ {},
    /* 0x71 */ { &CPU::ADC, AddressMode::IndirectIndexed, 2, 5, 1 },
    /* 0x72 */ {},
    /* 0x73 */ {},
    /* 0x74 */ {},
    /* 0x75 */ { &CPU::ADC, AddressMode::ZeroPageX, 2, 4, 0 },
    /* 0x76 */ {},
    /* 0x77 */ {},
    /* 0x78 */ {},
    /* 0x79 */ { &CPU::ADC, AddressMode::AbsoluteY, 3, 4, 1 },
    /* 0x7A */ {},
    /* 0x7B */ {},
    /* 0x7C */ {},
    /* 0x7D */ { &CPU::ADC, AddressMode::AbsoluteX, 3, 4, 1 },
    /* 0x7E */ {},
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
    /* 0x8A */ { &CPU::TXA, AddressMode::Implicit, 1, 2, 0 },
    /* 0x8B */ {},
    /* 0x8C */ { &CPU::STY, AddressMode::Absolute, 3, 4, 0 },
    /* 0x8D */ { &CPU::STA, AddressMode::Absolute, 3, 4, 0 },
    /* 0x8E */ { &CPU::STX, AddressMode::Absolute, 3, 4, 0 },
    /* 0x8F */ {},

    /* 0x90 */ {},
    /* 0x91 */ { &CPU::STA, AddressMode::IndirectIndexed, 2, 6, 0 },
    /* 0x92 */ {},
    /* 0x93 */ {},
    /* 0x94 */ { &CPU::STY, AddressMode::ZeroPageX, 2, 4, 0 },
    /* 0x95 */ { &CPU::STA, AddressMode::ZeroPageX, 2, 4, 0 },
    /* 0x96 */ { &CPU::STX, AddressMode::ZeroPageY, 2, 4, 0 },
    /* 0x97 */ {},
    /* 0x98 */ { &CPU::TYA, AddressMode::Implicit, 1, 2, 0 },
    /* 0x99 */ { &CPU::STA, AddressMode::AbsoluteY, 3, 5, 0 },
    /* 0x9A */ { &CPU::TSX, AddressMode::Implicit, 1, 2, 0 },
    /* 0x9B */ {},
    /* 0x9C */ {},
    /* 0x9D */ { &CPU::STA, AddressMode::AbsoluteX, 3, 5, 0 },
    /* 0x9E */ {},
    /* 0x9F */ {},

    /* 0xA0 */ { &CPU::LDY, AddressMode::Immediate, 2, 2, 0 },
    /* 0xA1 */ { &CPU::LDA, AddressMode::IndexedIndirect, 2, 6, 0 },
    /* 0xA2 */ { &CPU::LDX, AddressMode::Immediate, 2, 2, 0},
    /* 0xA3 */ {},
    /* 0xA4 */ {},
    /* 0xA5 */ { &CPU::LDA, AddressMode::ZeroPage, 2, 3, 0 },
    /* 0xA6 */ {},
    /* 0xA7 */ {},
    /* 0xA8 */ { &CPU::TAY, AddressMode::Implicit, 1, 2, 0 },
    /* 0xA9 */ { &CPU::LDA, AddressMode::Immediate, 2, 2, 0 },
    /* 0xAA */ { &CPU::TAX, AddressMode::Implicit, 1, 2, 0 },
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
    /* 0xB6 */ { &CPU::LDX, AddressMode::ZeroPageY, 2, 4, 0 },
    /* 0xB7 */ {},
    /* 0xB8 */ {},
    /* 0xB9 */ { &CPU::LDA, AddressMode::AbsoluteY, 3, 4, 1 },
    /* 0xBA */ { &CPU::TSX, AddressMode::Implicit, 1, 2, 0 },
    /* 0xBB */ {},
    /* 0xBC */ {},
    /* 0xBD */ { &CPU::LDA, AddressMode::AbsoluteX, 3, 4, 1 },
    /* 0xBE */ {},
    /* 0xBF */ {},

    /* 0xC0 */ { &CPU::CPY, AddressMode::Immediate, 2, 2, 0 },
    /* 0xC1 */ {},
    /* 0xC2 */ {},
    /* 0xC3 */ {},
    /* 0xC4 */ {},
    /* 0xC5 */ {},
    /* 0xC6 */ { &CPU::DEC, AddressMode::ZeroPage, 2, 5, 0 },
    /* 0xC7 */ {},
    /* 0xC8 */ { &CPU::INY, AddressMode::Implicit, 1, 2, 0 },
    /* 0xC9 */ { &CPU::CMP, AddressMode::Immediate, 2, 2, 0 },
    /* 0xCA */ { &CPU::DEX, AddressMode::Implicit, 1, 2, 0 },
    /* 0xCB */ {},
    /* 0xCC */ {},
    /* 0xCD */ {},
    /* 0xCE */ { &CPU::DEC, AddressMode::Absolute, 3, 6, 0 },
    /* 0xCF */ {},

    /* 0xD0 */ {},
    /* 0xD1 */ {},
    /* 0xD2 */ {},
    /* 0xD3 */ {},
    /* 0xD4 */ {},
    /* 0xD5 */ {},
    /* 0xD6 */ { &CPU::DEC, AddressMode::ZeroPageX, 2, 6, 0 },
    /* 0xD7 */ {},
    /* 0xD8 */ {},
    /* 0xD9 */ {},
    /* 0xDA */ {},
    /* 0xDB */ {},
    /* 0xDC */ {},
    /* 0xDD */ {},
    /* 0xDE */ { &CPU::DEC, AddressMode::AbsoluteX, 3, 7, 0 },
    /* 0xDF */ {},

    /* 0xE0 */ { &CPU::CPX, AddressMode::Immediate, 2, 2, 0 },
    /* 0xE1 */ { &CPU::SBC, AddressMode::IndexedIndirect, 2, 6, 0 },
    /* 0xE2 */ {},
    /* 0xE3 */ {},
    /* 0xE4 */ {},
    /* 0xE5 */ { &CPU::SBC, AddressMode::ZeroPage, 2, 3, 0 },
    /* 0xE6 */ { &CPU::INC, AddressMode::ZeroPage, 2, 5, 0 },
    /* 0xE7 */ {},
    /* 0xE8 */ { &CPU::INX, AddressMode::Implicit, 1, 2, 0 },
    /* 0xE9 */ { &CPU::SBC, AddressMode::Immediate, 2, 2, 0 },
    /* 0xEA */ { &CPU::NOP, AddressMode::Implicit, 1, 2, 0 },
    /* 0xEB */ {},
    /* 0xEC */ {},
    /* 0xED */ { &CPU::SBC, AddressMode::Absolute, 3, 4 , 0 },
    /* 0xEE */ { &CPU::INC, AddressMode::Absolute, 3, 6, 0 },
    /* 0xEF */ {},

    /* 0xF0 */ {},
    /* 0xF1 */ { &CPU::SBC, AddressMode::IndirectIndexed, 2, 5, 1 },
    /* 0xF2 */ {},
    /* 0xF3 */ {},
    /* 0xF4 */ {},
    /* 0xF5 */ { &CPU::SBC, AddressMode::ZeroPageX, 2, 4, 0 },
    /* 0xF6 */ { &CPU::INC, AddressMode::ZeroPageX, 2, 6, 0 },
    /* 0xF7 */ {},
    /* 0xF8 */ {},
    /* 0xF9 */ { &CPU::SBC, AddressMode::AbsoluteY, 3, 4, 1 },
    /* 0xFA */ {},
    /* 0xFB */ {},
    /* 0xFC */ {},
    /* 0xFD */ { &CPU::SBC, AddressMode::AbsoluteX, 3, 4, 1 },
    /* 0xFE */ { &CPU::INC, AddressMode::AbsoluteX, 3, 7, 0 },
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
    s = SetZN(s, x);
}

void CPU::LDY(Operand const& operand)
{
    y = memoryBus->Read(operand.address);
    s = SetZN(s, y);
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
    x = a;
    s = SetZN(s, x);
}

void CPU::TAY(Operand const&)
{
    y = a;
    s = SetZN(s, y);
}

void CPU::TXA(Operand const&)
{
    a = x;
    s = SetZN(s, a);
}

void CPU::TYA(Operand const&)
{
    a = y;
    s = SetZN(s, a);
}

// Stack Operations

void CPU::TSX(Operand const&)
{
    x = sp;
    s = SetZN(s, x);
}

void CPU::TXS(Operand const&)
{
    sp = x;
}

void CPU::PHA(Operand const&)
{
    Push(a);
}

void CPU::PLA(Operand const&)
{
    a = Pop();
    s = SetZN(s, a);
}

// Some consideration needed with flags when pushed to and popped from stack
// https://wiki.nesdev.com/w/index.php/Status_flags#The_B_flag
// I think I'll see how things go with this one when I run actual test ROMs
void CPU::PHP(Operand const&)
{
    Push(s);
}

void CPU::PLP(Operand const&)
{
    s = Pop();
}

// Logical

void CPU::AND(Operand const& operand)
{
    uint8_t value = memoryBus->Read(operand.address);
    a = a & value;
    s = SetZN(s, a);
}

void CPU::EOR(Operand const& operand)
{
    uint8_t value = memoryBus->Read(operand.address);
    a = a ^ value;
    s = SetZN(s, a);
}

void CPU::ORA(Operand const& operand)
{
    uint8_t value = memoryBus->Read(operand.address);;
    a = a | value;
}

void CPU::BIT(Operand const&)
{
}

// Arithmetic

// http://www.righto.com/2012/12/the-6502-overflow-flag-explained.html
// http://www.6502.org/tutorials/vflag.html
// Detecting potential signed overflow is the trickiest part of this instruction.
// I've seen a few different ways of doing it, but the docs above help UNDERSTAND it.
void CPU::ADC(Operand const& operand)
{
    uint16_t const m = a;
    uint16_t const n = memoryBus->Read(operand.address);
    uint16_t const c = (s & C) ? 1 : 0;
    uint16_t const result = m + n + c;

    s = SetZN(s, static_cast<uint8_t>(result));
    s = SetFlag(s, C, result > 0xFF);

    // Signed overflow?
    // If the signs of m and n are the same
    // And the sign of a and result are different
    s = SetFlag(s, V, (~(n ^ m) & (a ^ result)) & 0x0080);

    a = static_cast<uint8_t>(result);
}

// https://stackoverflow.com/questions/48971814/i-dont-understand-whats-going-on-with-sbc#
// https://www.c64-wiki.com/wiki/SBC
void CPU::SBC(Operand const& operand)
{
    uint16_t const m = a;
    uint16_t const n = memoryBus->Read(operand.address);
    uint16_t const c = (s & C) ? 1 : 0;
    uint16_t const result = m + ~n +  c;

    s = SetZN(s, static_cast<uint8_t>(result));
    s = SetFlag(s, C, result < 0xFF);
    s = SetFlag(s, V, (~(n ^ ~m) & (a ^ result)) & 0x0080);

    a = static_cast<uint8_t>(result);
}

static uint8_t Compare(uint8_t a, uint8_t b, uint8_t status)
{
    status = SetZN(status, a - b);
    status = SetFlag(status, C, a >= b);
    return status;
}

void CPU::CMP(Operand const& operand)
{
    auto const m = memoryBus->Read(operand.address);
    s = Compare(a, m, s);
}

void CPU::CPX(Operand const& operand)
{
    auto const m = memoryBus->Read(operand.address);
    s = Compare(x, m, s);
}

void CPU::CPY(Operand const& operand)
{
    auto const m = memoryBus->Read(operand.address);
    s = Compare(y, m, s);
}

// Increments & Decrements

void CPU::INC(Operand const& operand)
{
    auto value = memoryBus->Read(operand.address);
    value++;
    memoryBus->Write(operand.address, value);
    s = SetZN(s, value);
}

void CPU::INX(Operand const&)
{
    x++;
    s = SetZN(s, x);
}

void CPU::INY(Operand const&)
{
    y++;
    s = SetZN(s, y);
}

void CPU::DEC(Operand const& operand)
{
    auto value = memoryBus->Read(operand.address);
    value--;
    memoryBus->Write(operand.address, value);
    s = SetZN(s, value);
}

void CPU::DEX(Operand const&)
{
    x--;
    s = SetZN(s, x);
}

void CPU::DEY(Operand const&)
{
    y--;
    s = SetZN(s, y);
}

// Shifts

void CPU::ASL(Operand const& operand)
{
    if (operand.addressMode == AddressMode::Accumulator)
    {
        s = SetFlag(s, C, a & 0x80);
        a <<= 1;
        s = SetZN(s, a);
    }
    else
    {
        auto value = memoryBus->Read(operand.address);
        s = SetFlag(s, C, value & 0x80);
        value <<= 1;
        s = SetZN(s, value);
        memoryBus->Write(operand.address, value);
    }
}

void CPU::LSR(Operand const&)
{
}

void CPU::ROL(Operand const& operand)
{
    uint8_t currentCarry = s & 0x01;
    if (operand.addressMode == AddressMode::Accumulator)
    {
        s = SetFlag(s, C, a & 0x80);
        a <<= 1;
        a |= currentCarry;
        s = SetZN(s, a);
    }
    else
    {
        auto value = memoryBus->Read(operand.address);
        s = SetFlag(s, C, value & 0x80);
        value <<= 1;
        value |= currentCarry;
        s = SetZN(s, value);
        memoryBus->Write(operand.address, value);
    }
}

void CPU::ROR(Operand const&)
{
}

// Jumps & Calls

void CPU::JMP(Operand const& operand)
{
    pc = operand.address;
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
    s &= ~C;
}

void CPU::SEC(Operand const&)
{
    s |= C;
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
