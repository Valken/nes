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
    
    Decode(instructionInfo.addressMode);
    
    Operand operand;
    std::invoke(instructionInfo.execute, this, operand);

    // Return num cycles
}

uint8_t CPU::Fetch()
{
    return MemoryBus->Read(PC);
}

void CPU::Decode(AddressMode addressMode)
{
}

void CPU::NOP(Operand const&)
{
}

} // nes
