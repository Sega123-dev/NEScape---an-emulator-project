#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <vector>
#include "nrom.hpp"
#include "../../ppu/ricoh2c02ntsc.hpp"
#include "../../bus/bus.hpp"

void NROM::reset()
{
    prg16kb = false;
    chr8kb = true;
}

NROM::NROM(const std::vector<uint8_t> &prgData, const std::vector<uint8_t> &chrData, const bool &mirrorPortVertical)
{
    prg = prgData;
    chr = chrData;

    if (prg.size() == 0x4000)
        prg16kb = true;
    else
        prg16kb = false;

    if (chr.size() == 0)
        chr8kb = false;
    else
        chr8kb = true;

    if (!chr8kb)
        chr.resize(0x2000);

    prgRam.resize(0x2000);
    if (!mirrorPortVertical)
        mirroring = HORIZONTAL;
    else
        mirroring = VERTICAL;
}

uint8_t NROM::cpuRead(uint16_t addr)
{
    uint8_t value;
    if (addr >= 0x6000 && addr <= 0x7FFF)
    {
        value = prgRam[addr - 0x6000];
    }
    else if (addr >= 0x8000)
    {
        uint16_t index = addr - 0x8000;
        if (prg16kb)
            index %= 0x4000;
        else
            index %= prg.size();
        value = prg[index];
    }
    else
    {
        value = lastCpuValue;
    }
    lastCpuValue = value;
    return value;
}
uint8_t NROM::ppuRead(uint16_t addr)
{
    return addr < 0x2000 ? chr[addr] : 0;
}

void NROM::ppuWrite(uint16_t addr, uint8_t data)
{
    if (addr < 0x2000 && !chr8kb) // only writable if it's CHR-RAM
        chr[addr] = data;
}
void NROM::cpuWrite(uint16_t addr, uint8_t data)
{
    if (addr >= 0x6000 && addr <= 0x7FFF)
        prgRam[addr - 0x6000] = data;
    lastCpuValue = data;
}
// TEMP FIX
/* uint16_t NROM::mirror(uint16_t addr)
{
    addr = addr & 0x0FFF;
    if (mirroring == VERTICAL)
    {
        return addr % 0x800;
    }
    else if (mirroring == HORIZONTAL)
    {
        return (addr % 0x400) + (addr / 0x800) * 0x400;
    }
    return 0;
}
*/
Mirror NROM::getMirror()
{
    if (mirroring == HORIZONTAL)
    {
        return Mirror::HORIZONTAL;
    }
    else
    {
        return Mirror::VERTICAL;
    }
}
void NROM::connectNrom(Bus *b)
{
    bus = *b;
}