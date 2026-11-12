#include "mmc1.hpp"
#include <iostream>
#include <cstdint>
#include <vector>
#include "../../bus/bus.hpp"
#include "../iNES_reader/reader.hpp"

uint8_t prgRam[0x2000];
void MMC1::reset()
{
    shiftReg = 0x10;
    shiftCount = 0;
    controlReg = 0x0C;
    chrBank0 = chrBank1 = prgBank = 0;
}

MMC1::MMC1(const std::vector<uint8_t> &prgData, const std::vector<uint8_t> &chrData,
           const bool &lowHighMirrorPort, const bool &mirrorPortVertical)
{
    prg = prgData;
    chr = chrData;
    reset();
    mirroring = mirrorPortVertical ? VERTICAL : HORIZONTAL;
}

void MMC1::cpuWrite(uint16_t addr, uint8_t value)
{
    if (addr < 0x8000)
    {
        if (addr >= 0x6000)
            prgRam[addr - 0x6000] = value;
        return;
    }
    if (value & 0x80)
    {
        controlReg |= 0x0C;
        shiftReg = 0x10;
        shiftCount = 0;
        return;
    }

    shiftReg = (shiftReg >> 1) | ((value & 1) << 4);
    if (++shiftCount < 5)
        return;

    uint8_t v = shiftReg & 0x1F;
    shiftReg = 0x10;
    shiftCount = 0;

    switch ((addr >> 13) & 3)
    {
    case 0:
        controlReg = v;
        switch (v & 3)
        {
        case 0:
            mirroring = ONESCREENLOW;
            break;
        case 1:
            mirroring = ONESCREENHIGH;
            break;
        case 2:
            mirroring = VERTICAL;
            break;
        default:
            mirroring = HORIZONTAL;
            break;
        }
        break;
    case 1:
        chrBank0 = v;
        break;
    case 2:
        chrBank1 = v;
        break;
    default:
        prgBank = v;
        break;
    }
}

uint8_t MMC1::cpuRead(uint16_t addr)
{
    if (addr < 0x8000)
        return addr >= 0x6000 ? prgRam[addr - 0x6000] : 0;
    uint8_t mode = (controlReg >> 2) & 3, bank = prgBank & 0x0F;
    size_t index;
    if (mode < 2)
        index = (bank & 0x0E) * 0x4000 + (addr - 0x8000);
    else if (mode == 2)
        index = addr < 0xC000 ? addr - 0x8000 : bank * 0x4000 + (addr - 0xC000);
    else
        index = addr < 0xC000 ? bank * 0x4000 + (addr - 0x8000) : prg.size() - 0x4000 + (addr - 0xC000);
    lastCpuValue = prg[index % prg.size()];
    return lastCpuValue;
}

static size_t chrIndex(uint8_t ctrl, uint8_t b0, uint8_t b1, uint16_t addr, size_t size)
{
    addr &= 0x1FFF;
    size_t i = !(ctrl & 0x10)  ? (size_t)(b0 & 0x1E) * 0x1000 + addr
               : addr < 0x1000 ? (size_t)b0 * 0x1000 + addr
                               : (size_t)b1 * 0x1000 + (addr - 0x1000);
    return i % size;
}
uint8_t MMC1::ppuRead(uint16_t addr) { return chr[chrIndex(controlReg, chrBank0, chrBank1, addr, chr.size())]; }
void MMC1::ppuWrite(uint16_t addr, uint8_t data)
{
    chr[chrIndex(controlReg, chrBank0, chrBank1, addr, chr.size())] = data; // CHR-ROM ignores writes
}
Mirror MMC1::getMirror() { return mirroring; }
void MMC1::connectMMC1(Bus *b)
{
    bus = *b;
}