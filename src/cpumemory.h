#pragma once
#include "memory.h"
#include <vector>

namespace nes
{

class CPUMemory : public Memory
{
public:
    CPUMemory(std::vector<uint8_t>& ram);
    
    uint8_t Read(uint16_t address) override;
    void Write(uint16_t address, uint8_t value) override;
    
private:
    std::vector<uint8_t>& ram;
};

} // nes
