#include "axrom.hpp"
#include <cstdint>

void AXROM::reset()
{
    lastCpuValue = 0;
    lastPpuValue = 0;
    prgBanks = 0;
    currentBank = 0;
    mirroring = ONESCREENLOW;
}

AXROM::AXROM(const std::vector<uint8_t> &prgData, const std::vector<uint8_t> &chrData)
{
    prg = prgData;
    chr = chrData;

    prgBanks = prg.size() / 0x8000;
    currentBank = 0;
}

uint8_t AXROM::cpuRead(uint16_t addr)
{
    if (addr >= 0x8000)
        return prg[currentBank * 0x8000 + (addr & 0x7FFF)];

    return 0;
}

void AXROM::cpuWrite(uint16_t addr, uint8_t data)
{
    if (addr >= 0x8000 && addr <= 0xFFFF)
    {
        currentBank = data & 0x07;
        mirroring = (data & 0x10) ? ONESCREENHIGH : ONESCREENLOW;
    }
}

uint8_t AXROM::ppuRead(uint16_t addr)
{
    if (addr < 0x2000)
    {
        return chr[addr & 0x1FFF];
    }

    return 0;
}

void AXROM::ppuWrite(uint16_t addr, uint8_t data)
{
    if (addr < 0x2000)
    {
        chr[addr & 0x1FFF] = data;
    }
}

Mirror AXROM::getMirror()
{
    if (mirroring == ONESCREENLOW)
    {
        return Mirror::ONESCREENLOW;
    }
    else
    {
        return Mirror::ONESCREENHIGH;
    }
}