#include <stdint.h>
#include <fstream>
#include <string>
#include <vector>
#include "cpumemory.h"
#include "cpu.h"

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

int main(int argc, char *argv[])
{
    std::vector<uint8_t> ram;
    ram.reserve(0x2000);
    nes::CPUMemory memoryMap(ram);
    nes::CPU cpu(&memoryMap);
    
    /*auto cycles =*/ cpu.Step();
    
    if (argc < 1)
        return 1;
    
    auto filename = std::string(argv[1]);
    auto fileStream = std::ifstream(filename, std::ifstream::binary);
    
    Header header = {};
    fileStream.read(reinterpret_cast<char *>(&header), sizeof(header));
    
    size_t romSize = header.ProgRomCount * ProgRomBankSize;
    auto rom = std::vector<uint8_t>(romSize);
    size_t vromSize = header.ChrRomCount * ChrRomBankSize;
    auto vrom = std::vector<uint8_t>(vromSize);
    
    size_t readCount = 0;
    readCount = fileStream.read(reinterpret_cast<char *>(rom.data()), romSize).gcount();
    readCount = fileStream.read(reinterpret_cast<char *>(vrom.data()), vromSize).gcount();
    
    // Some temp code to  check if we've read everything!
    if (!fileStream.eof())
    {
        printf("Didn't get to EOF while reading %s\n", filename.c_str());
        char buff[1024];
        readCount = fileStream.read(buff, 1024).gcount();
        auto isEofNow = fileStream.eof();
        printf("After reading %ld bytes, eof is now %s", readCount, isEofNow ? "true" : "false");
    }
    
//    if (int a = 0; a == 0)
//    {
//        printf("YES!\n");
//    }
//
//    auto [one, two, three] = std::make_tuple(1, "two", 3.0f);
    return 0;
}
