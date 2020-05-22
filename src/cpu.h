#pragma once
#include <stdint.h>
#include "memory.h"

namespace nes
{

enum class AddressMode;
struct Operand;

struct CPU
{
    uint16_t PC;
    uint8_t A;
    uint8_t X;
    uint8_t Y;
    uint8_t Status;
    Memory* const MemoryBus;

    explicit CPU(Memory* const memory);
    ~CPU() = default;
    // Rule of 5 here?

    void Step();
    
    void NOP(Operand const& operand); // This shouldn't be public...

private:
    uint8_t Fetch(); // Read current opcode from PC, advance PC by instruction size
    Operand Decode(AddressMode addressMode) const;
};

} // nes
