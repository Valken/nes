#include <stdint.h>
#include <fstream>
#include <string>
#include <vector>
#include "Memory.h"

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

// So I'll need to sort a memory map, but also console ram, rom (and as such, the mapper inteface that sits in front of rom)
// Then I can... what, see if I can decode instructions...

//class CPUMemory : public Memory
//{
//public:
//    virtual ~CPUMemory() {}
//
//    uint8_t Read(uint16_t address) override;
//    void Write(uint16_t address, uint8_t value) override;
//};
//
//uint8_t CPUMemory::Read(uint16_t address)
//{
//    return 0;
//}
//
//void CPUMemory::Write(uint16_t address, uint8_t value)
//{
//
//}
void Thing() { printf("thing\n"); };
void OtherThing() { printf("otherthing\n"); };
void (*opcodes[])() { Thing, OtherThing };

struct Stuff
{
    int a;
    int b;
    int c;

    void Step();
    
    void Thing(int thing)
    {
        printf("thing\n");
    };
    
    void OtherThing(int someThing)
    {
        printf("otherthing\n");
    }
};

// POC for pointer to member jump table that doesn't go down the lambda or std::bind route
// Can call std::invoke or have a macro to call function on opcode lookup.
// Need to investigate std::invoke perf
typedef void (Stuff::*StuffMember)(int);
StuffMember things[]
{
    &Stuff::Thing,
    &Stuff::OtherThing
};

void Stuff::Step()
{
    std::invoke(things[0], this, 111);
}

int main(int argc, char *argv[])
{
    opcodes[0]();
    opcodes[1]();
    
    Stuff s;
    std::invoke(things[0], s, 222);
    std::invoke(things[1], s, 333);
    
    s.Step();
    
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
    
    if (int a = 0; a == 0)
    {
        printf("YES!\n");
    }
    
    Stuff stuff {.a = 1, .b = 2, .c = 3};
    
    auto [one, two, three] = std::make_tuple(1, "two", 3.0f);
    return 0;
}
