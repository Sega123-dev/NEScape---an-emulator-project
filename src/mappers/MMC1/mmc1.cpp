#include "mmc1.hpp"
#include <iostream>
#include <cstdint>
#include <vector>
#include "../../bus/bus.hpp"

// PRG RAM

uint8_t prgRam[0x2000];

void MMC1::reset()
{
    shiftReg = 0x10;
    shiftCount = 0;

    controlReg = 0x0C;
    chrBank0 = 0, chrBank1 = 0;
    prgBank = 0;

    mirroring = Mirror::HORIZONTAL;
}
MMC1::MMC1(const std::vector<uint8_t> &prgData, const std::vector<uint8_t> &chrData, const bool &lowHighMirrorPort, const bool &mirrorPortVertical)
{
    prg = prgData;
    chr = chrData;

    if (!verticalMirroring)
        mirroring = HORIZONTAL;
    else
        mirroring = VERTICAL;

    if (!lowHighMirrorPort)
        mirroring = HORIZONTAL;
    else
        mirroring = VERTICAL;

    reset();
};
void MMC1::cpuWrite(uint16_t addr, uint8_t value)
{
    if (addr < 0x8000)
    {
        prgRam[addr - 0x6000] = value;
        return;
    }

    if (addr < 0x8000)
        return;

    if (value & 0x80)
    {
        controlReg |= 0x0C;
        shiftReg = 0x10;
        shiftCount = 0;
        return;
    }

    shiftReg >>= 1;
    shiftReg |= (value & 1) << 4;
    shiftCount++;

    if (shiftCount == 5)
    {
        if (addr >= 0x8000 && addr <= 0x9FFF)
            controlReg = shiftReg & 0x1F;
        else if (addr >= 0xA000 && addr <= 0xBFFF)
            chrBank0 = shiftReg & 0x1F;
        else if (addr >= 0xC000 && addr <= 0xDFFF)
            chrBank1 = shiftReg & 0x1F;
        else if (addr >= 0xE000 && addr <= 0xFFFF)
            prgBank = shiftReg & 0x1F;
        // MIRRORING MIGRATED TO PPU
        switch (controlReg & 0x03)
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
        case 3:
            mirroring = HORIZONTAL;
            break;
        }
    };
}
uint8_t MMC1::cpuRead(uint16_t addr)
{
    uint8_t value;
    if (addr < 0x8000)
        return prgRam[addr - 0x6000];
    size_t index = 0;
    uint8_t prgMode = (controlReg >> 2) & 0x03;

    switch (prgMode)
    {
    case 0:
    case 1:
        index = ((prgBank & 0xFE) * 0x4000 + (addr - 0x8000)) % prg.size();
        value = prg[index];
        break;
    case 2:
        if (addr < 0xC000)
            index = (addr - 0x8000) % prg.size();
        else
            index = (prgBank * 0x4000 + (addr - 0xC000)) % prg.size();
        value = prg[index];
        break;
    case 3:
        if (addr < 0xC000)
            index = (prgBank * 0x4000 + (addr - 0x8000)) % prg.size();
        else
            index = (prg.size() - 0x4000 + (addr - 0xC000)) % prg.size();
        value = prg[index];
        break;
    }
    lastCpuValue = value;
    return value;
}
uint8_t MMC1::ppuRead(uint16_t addr)
{
    addr &= 0x1FFF;
    uint8_t chrMode = (controlReg >> 4) & 1;
    uint8_t bank = chrBank0 & 0x1E;
    size_t finalAddr = 0;
    if (chrMode == 0)
    {
        bank = chrBank0 & 0x1E;
        finalAddr = bank * 0x1000 + addr;
    }
    if (chrMode == 1)
    {
        if (addr < 0x1000)
            finalAddr = chrBank0 * 0x1000 + addr;
        else if (addr < 0x2000)
            finalAddr = chrBank1 * 0x1000 + (addr - 0x1000);
    }

    return chr[finalAddr % chr.size()];
}
void MMC1::ppuWrite(uint16_t addr, uint8_t data)
{
    addr &= 0x1FFF;
    uint8_t chrMode = (controlReg >> 4) & 1;
    uint8_t bank = chrBank0 & 0x1E;
    uint16_t finalAddr;
    if (chrMode == 0)
    {
        bank = chrBank0 & 0x1E;
        finalAddr = bank * 0x1000 + addr;
    }
    if (chrMode == 1)
    {
        if (addr < 0x1000)
            finalAddr = chrBank0 * 0x1000 + addr;
        else if (addr < 0x2000)
            finalAddr = chrBank1 * 0x1000 + (addr - 0x1000);
    }

    chr[finalAddr % chr.size()] = data;
}
Mirror MMC1::getMirror()
{
    // just to bypass the compiler error,not an actual implementation
    if (mirroring == HORIZONTAL)
    {
        return Mirror::HORIZONTAL;
    }
    else
    {
        return Mirror::VERTICAL;
    }
}
void MMC1::connectMMC1(Bus *b)
{
    bus = *b;
}