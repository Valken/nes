//
// Created by Neil Mackey on 31/01/2021.
//

#ifndef NES_CPUTESTS_H
#define NES_CPUTESTS_H

#include "../src/cpu.h"
#include <gtest/gtest.h>

class TestMemory : public nes::Memory
{
public:
    uint8_t data[0xFFFF] = { 0 };

    TestMemory()
    {
        // Set the program start address to 0x1000
        // Little endian so least significant bit first
        data[0xFFFC] = 0x00;
        data[0xFFFD] = 0x10;
    }

    uint8_t Read(uint16_t address) override
    {
        return data[address];
    }

    void Write(uint16_t address, uint8_t value) override
    {
        data[address] = value;
    }
};

class CpuTests : public ::testing::Test
{
public:
    nes::CPU cpu;
    TestMemory memory;

    CpuTests() : memory(), cpu(&memory)
    {
    }
};

#endif //NES_CPUTESTS_H
