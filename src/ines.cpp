#include "cartridge.h"

namespace
{

struct Header
{
    char Magic[4];
    uint8_t ProgRomCount;
    uint8_t ChrRomCount;
    uint8_t Flags6;
    uint8_t Flags7;
    uint8_t Flags8;
    uint8_t Flags9;
    uint8_t Flags10;
    uint8_t Padding[5];
};

static_assert(sizeof(Header) == 16, "Header should be 16 bytes!");
constexpr size_t ProgRomBankSize = 16 * 1024;
constexpr size_t ChrRomBankSize = 8 * 1024;

} //

using namespace nes;

Cartridge LoadFromFile(std::string fileName)
{
    
}
