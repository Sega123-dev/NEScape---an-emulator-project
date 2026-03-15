#include <iostream>
#include <cstdint>
#include <vector>
#include "uxrom.hpp"

Bus *bus = nullptr;

void UXROM::reset()
{
    currentPRGbank = 0;
}
UXROM::UXROM(std::vector<uint8_t> &prgData, std::vector<uint8_t> &chrData)
{
    chr = chrData;
    prg = prgData;

    currentPRGbank = 0;
}
uint8_t UXROM::ppuRead(uint16_t addr)
{
    uint8_t value;
    if (addr < 0x2000)
        value = chr[addr];
    else
        value = lastPpuValue;

    lastPpuValue = value;
    return value;
}
void UXROM::cpuWrite(uint16_t addr, uint8_t data)
{
    if (addr >= 0x8000)
    {
        size_t bankCount = prg.size() / 0x4000;
        if (bankCount > 1)
            currentPRGbank = data % (bankCount - 1);
        else
            currentPRGbank = 0;
        currentPRGbank = data % (bankCount - 1);
    }
}
uint8_t UXROM::cpuRead(uint16_t addr)
{
    uint8_t value;
    if (addr >= 0x8000 && addr <= 0xBFFF)
    {
        value = prg[(addr - 0x8000 + currentPRGbank * 0x4000) % prg.size()];
    }
    else if (addr >= 0xC000 && addr <= 0xFFFF)
    {
        value = prg[addr - 0xC000 + (prg.size() / 0x4000 - 1) * 0x4000];
    }
    else
        value = lastCpuValue;

    lastCpuValue = value;
    return value;
}
void UXROM::ppuWrite(uint16_t addr, uint8_t data)
{
    std::cout << "No ram writes for UXROM" << std::endl;
}

Mirror UXROM::getMirror() {}

void UXROM::connectUxrom(Bus *b)
{
    bus = b;
}