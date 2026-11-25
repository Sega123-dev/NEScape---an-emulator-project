#include <cstdint>
#include "color_dreams.hpp"

Color_Dreams::Color_Dreams(const std::vector<uint8_t> &prgData, const std::vector<uint8_t> &chrData)
{
    prg = prgData;
    chr = chrData;

    lastCpuValue = 0;
    lastPpuValue = 0;

    prgBanks = prg.size() / 0x8000;
    chrBanks = chr.size() / 0x2000;
    currentPRGBank = 0;
    currentCHRBank = 0;

    mirroring = HORIZONTAL;
}

void Color_Dreams::reset()
{
    lastCpuValue = 0;
    lastPpuValue = 0;

    prgBanks = prg.size() / 0x8000;
    chrBanks = prg.size() / 0x2000;
    currentPRGBank = 0;
    currentCHRBank = 0;

    mirroring = HORIZONTAL;
}

void Color_Dreams::cpuWrite(uint16_t addr, uint8_t data)
{
    if (addr >= 0x8000 && addr <= 0xFFFF)
    {
        currentPRGBank = (data & 0x03) % prgBanks;
        currentCHRBank = ((data >> 4) & 0x0F) % chrBanks;
    }
}

uint8_t Color_Dreams::cpuRead(uint16_t addr)
{
    if (addr >= 0x8000 && addr <= 0xFFFF)
    {
        uint32_t offset = currentPRGBank * 0x8000 + (addr - 0x8000);
        return prg[offset];
    }
    return 0;
}

void Color_Dreams::ppuWrite(uint16_t addr, uint8_t data)
{
    // No PPU WRITE(Color dreams is a ROM only mapper)
}

uint8_t Color_Dreams::ppuRead(uint16_t addr)
{
    if (addr < 0x2000)
    {
        uint32_t offset = currentCHRBank * 0x2000 + addr;
        return chr[offset];
    }

    return 0;
}

Mirror Color_Dreams::getMirror()
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