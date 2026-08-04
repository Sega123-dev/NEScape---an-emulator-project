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
        horizontalMirroring = HORIZONTAL;
    }
    else
    {
        horizontalMirroring = VERTICAL;
    }
}
uint8_t UXROM::ppuRead(uint16_t addr)
{
    uint8_t value = lastPpuValue;
    if (addr < 0x2000)
    {
        value = chr[addr];
    }

    else if (addr >= 0x2000 && addr <= 0x3EFF)
    {
        addr &= 0x0FFF;

        uint16_t vramIndex;
        uint8_t offset = addr % 0x400;
        uint8_t ntIndex = addr / 0x400;

        if (horizontalMirroring)
        {
            vramIndex = (ntIndex < 2) ? 0 : 1;
        }
        else
        {
            vramIndex = (ntIndex % 2 == 0) ? 0 : 1;
        }
        value = bus.ppu->ppuReadRaw(vramIndex * 0x400 + offset);
    };

    lastPpuValue = value;
    return value;
}
void UXROM::ppuWrite(uint16_t addr, uint8_t data)
{
    if (addr < 0x2000)
    {
        chr[addr] = data;
    }

    else if (addr >= 0x2000 && addr <= 0x3EFF)
    {
        addr &= 0x0FFF;
        uint16_t vramIndex;
        uint8_t ntIndex = addr / 0x400;
        uint8_t offset = addr % 0x400;

        if (horizontalMirroring)
        {
            vramIndex = (ntIndex < 2) ? 0 : 1;
        }
        else
        {
            vramIndex = (ntIndex % 2 == 0) ? 0 : 1;
        }
        bus.ppu->ppuWriteRaw(vramIndex * 0x400 + offset, data);
    }
}
void UXROM::cpuWrite(uint16_t addr, uint8_t data)
{
    if (addr >= 0x8000)
    {
        std::cout << "BANK SWITCH: " << std::hex << (int)data << "\n";
        currentPRGbank = data & 0x0F;
        currentPRGbank %= (prg.size() / 0x4000 - 1);
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
    return horizontalMirroring ? HORIZONTAL : VERTICAL;
}

void UXROM::connectUxrom(Bus *b)
{
    bus = *b;
}