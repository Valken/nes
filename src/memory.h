#pragma once
#include <cstdint>

namespace nes
{

class Memory
{
public:
    virtual ~Memory() = default;
    virtual uint8_t Read(uint16_t address) = 0;
    virtual void Write(uint16_t address, uint8_t value) = 0;
};

}