#include "cpumemory.h"

namespace nes
{

CPUMemory::CPUMemory(std::vector<uint8_t>& ram) : ram(ram)
{
}

uint8_t CPUMemory::Read(uint16_t address)
{
    if (address < 0x2000)
    {
        return ram[address % 0x800];
    }
    else if (address >= 0x2000 && address < 0x4000)
    {
        // TODO!
    }
    else
    {
        // TODO!
    }
}

void CPUMemory::Write(uint16_t address, uint8_t value)
{
}

} // nes
