#include <stdint.h>
#include <fstream>
#include <string>

struct Header
{
    char Magic[4];
    uint8_t ProgRomSize;
    uint8_t ChrRomSize;
    uint8_t Flags6;
    uint8_t Flags7;
    uint8_t Flags8;
    uint8_t Flags9;
    uint8_t Flags10;
    uint8_t Padding[5];
};

static_assert(sizeof(Header) == 16, "Header should be 16 bytes!");

int main(int argc, char *argv[])
{
    if (argc < 1)
        return 1;
    
    auto filename = std::string(argv[1]);
    auto fileStream = std::ifstream(filename, std::ifstream::binary);
    
    Header header = {};
    fileStream.read(reinterpret_cast<char *>(&header), sizeof(header));
    
    return 0;
}
