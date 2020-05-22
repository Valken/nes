#include <functional>
#include "cpu.h"
#include "memory.h"

namespace nes
{

enum class AddressMode
{
    Implicit,
    Accumulator,
    Immediate,
    ZeroPage,
    ZeroPageX,
    ZeroPageY,
    Relative,
    Absolute,
    AbsoluteX,
    AbsoluteY,
    Indirect,
    IndexedIndirect,
    IndirectIndexed,
};

}

namespace {

typedef void (nes::CPU::*Execute)(nes::Operand const&);

struct InstructionInfo
{
    Execute execute;
    nes::AddressMode addressMode;
    uint8_t instructionSize;
    // Cycles
};

InstructionInfo InstructionInfo[256] =
{
    { &nes::CPU::NOP, nes::AddressMode::Implicit, 1, },
};

// InstructionInfo table here

} // anonymous

namespace nes
{

struct Operand
{
    uint16_t value;
    uint16_t address;
    AddressMode addressMode;
};

CPU::CPU(Memory* const memoryBus) : PC(0x0000), A(0), X(0), Y(0), MemoryBus(memoryBus)
{
}

void CPU::Step()
{
    // Interrupt?

    auto programCounter = PC;
    auto instruction = Fetch();
    auto instructionInfo = InstructionInfo[instruction];
    PC += instructionInfo.instructionSize;
    
    auto operand = Decode(instructionInfo.addressMode);
    
    std::invoke(instructionInfo.execute, this, operand);

    // Return num cycles
}

uint8_t CPU::Fetch()
{
    return MemoryBus->Read(PC);
}

Operand CPU::Decode(AddressMode addressMode) const
{
    return Operand { .value = 0x0, .address = 0x0, .addressMode = AddressMode::Indirect };
}

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

void CPU::LSL(Operand const&)
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
