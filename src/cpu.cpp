#include <functional>
#include "cpu.h"
#include "Memory.h"

namespace {

typedef void (CPU::*Execute)(Operand const&);

struct InstructionInfo
{
    Execute execute;
    AddressMode addressMode;
    // Size
    // Cycles
};

InstructionInfo InstructionInfo[256] =
{
};

// InstructionInfo table here

} // anonymous

enum class AddressMode
{
    Direct,
    Indirect,
    Etc,
};

struct Operand
{
};

CPU::CPU(Memory *const memoryBus) : MemoryBus(memoryBus)
{
}

void CPU::Step()
{
    // Interrupt?
    
    auto programCounter = PC;
    auto instruction = Fetch();
    auto instructionInfo = InstructionInfo[instruction];
    
    Decode(instructionInfo.addressMode);
    Operand operand;
    std::invoke(instructionInfo.execute, this, operand);
    
    // Decode
    // Execute
    
    // Return num cycles
}

uint8_t CPU::Fetch()
{
    return MemoryBus->Read(PC);
}

void CPU::Decode(AddressMode addressMode)
{
}
