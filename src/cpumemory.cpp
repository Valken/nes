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
    else if (address < 0x4000)
    {
        //auto ppuRegister = address % 8;
        return 0x00;
    }
    else if (address < 0x6000)
    {
        // Lots of more data mapping to do here
    }
    else if (address >= 0x6000)
    {
        // Mapper
    }

    return 0x00;
}

void CPUMemory::Write(uint16_t address, uint8_t value)
{
    if (address < 0x2000)
    {
        ram[address % 0x800] = value;
    }
    else if (address < 0x4000)
    {
        // PPU register write
    }
    else if (address < 0x6000)
    {
        // Lots of more data mapping to do here
    }
    else if (address >= 0x6000)
    {
        // Mapper
    }
}

} // nes
