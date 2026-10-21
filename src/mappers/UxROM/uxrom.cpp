#include <iostream>
#include <cstdint>
#include <vector>
#include "uxrom.hpp"

void UXROM::reset()
{
    currentPRGbank = 0;
}
UXROM::UXROM(std::vector<uint8_t> &prgData, std::vector<uint8_t> &chrData, const bool &vertMirroringPort)
{
    chr = chrData;
    prg = prgData;

    currentPRGbank = 0;

    if (!vertMirroringPort)
    {
        mirroring = HORIZONTAL;
    }
    else
    {
        mirroring = VERTICAL;
    }
}
// PPU owns the mirroring
uint8_t UXROM::ppuRead(uint16_t addr)
{
    return addr < 0x2000 ? chr[addr] : 0;
}
void UXROM::ppuWrite(uint16_t addr, uint8_t data)
{
    if (addr < 0x2000)
        chr[addr] = data;
}
void UXROM::cpuWrite(uint16_t addr, uint8_t data)
{
    if (addr >= 0x8000)
    {
        currentPRGbank = data & 0x0F;
        currentPRGbank = data % (prg.size() / 0x4000);
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

Mirror UXROM::getMirror()
{
    if (mirroring == HORIZONTAL)
        return Mirror::HORIZONTAL;
    else
        return Mirror::VERTICAL;
}

void UXROM::connectUxrom(Bus *b)
{
    bus = *b;
}