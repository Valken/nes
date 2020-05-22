#include <functional>
#include "cpu.h"
#include "memory.h"

namespace nes
{

enum class AddressMode
{
    Direct,
    Indirect,
    Etc,
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
    { &nes::CPU::NOP, nes::AddressMode::Direct, 1, },
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

void CPU::NOP(Operand const&)
{
}

} // nes
