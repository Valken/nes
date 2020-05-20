#include "Memory.h"

class CPUMemory : public Memory
{
public:
    uint8_t Read(uint16_t address) override;
    void Write(uint16_t address, uint8_t value) override;
};

uint8_t CPUMemory::Read(uint16_t)
{
    return 0;
}

void CPUMemory::Write(uint16_t address, uint8_t value)
{
    
}
