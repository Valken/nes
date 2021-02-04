//
// Created by Neil Mackey on 31/01/2021.
//

#ifndef NES_CPUTESTS_H
#define NES_CPUTESTS_H

#include "../src/cpu.h"
#include <gtest/gtest.h>
#include <span>

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

#if 0
    template<size_t N>
    void WriteProgram(uint8_t (&program)[N])
    {
        constexpr uint16_t offset = 0x1000;
        for (size_t i = 0; i < N; i++)
        {
            data[offset + i] = program[i];
        }
    }
#else
    void WriteProgram(std::span<uint8_t> program)
    {
        if (program.size() > 0xFFFF - 0x1000) return;

        constexpr size_t offset = 0x1000;
        size_t count = 0;
        std::for_each(program.begin(), program.end(), [&count, this](uint8_t b)
        {
            data[offset + count] = b;
            count++;
        });
    }
#endif
};

class CpuTests : public ::testing::Test
{
public:
    TestMemory memory;
    nes::CPU cpu;

    CpuTests() : memory(), cpu(&memory)
    {
    }
};

#endif //NES_CPUTESTS_H
