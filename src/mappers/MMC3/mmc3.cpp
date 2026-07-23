#include "mmc3.hpp"
#include <iostream>
#include <cstdint>
#include <vector>
#include "../../bus/bus.hpp"
#include "../../ppu/ricoh2c02ntsc.hpp"
PPU *ppu;
void MMC3::reset()
{
    for (int i = 0; i < 8; i++)
        bankRegister[i] = 0;
    for (int i = 0; i < 4; i++)
        prgBankMap[i] = 0;
    for (int i = 0; i < 8; i++)
        chrBankMap[i] = 0;

    bankRegister[6] = 0;
    bankRegister[7] = 1;

    prgMode = 0;
    chrMode = 0;

    bankSelect = 0;

    mirrorHorizontal = true;
    wramEnabled = false;
    wramWriteProtected = false;
    irqCounter = 0;
    irqReloadValue = 0;
    irqEnabled = false;
    irqReloadFlag = false;
    if (ppu)
    {
        prevA12 = (ppu->getLastAddress() & 0x1000) >> 12;
    }
    else
    {
        prevA12 = 0;
    }

    updatePRGBanks();
    updateCHRBanks();
}

MMC3::MMC3(std::vector<uint8_t> &prgData, std::vector<uint8_t> &chrData, const bool &vertMirrorPort)
{
    prg = prgData;
    chr = chrData;

    if (!vertMirrorPort)
    {
        mirrorHorizontal = true;
    }
    else
    {
        mirrorHorizontal = false;
    }

    wram.resize(0x2000, 0);
    reset();
}
void MMC3::updatePRGBanks()
{
    int lastBank = (prg.size() / 0x2000) - 1;
    int secondLastBank = lastBank - 1;

    uint8_t r6 = bankRegister[6] % (prg.size() / 0x2000);
    uint8_t r7 = bankRegister[7] % (prg.size() / 0x2000);

    if (prgMode == 0)
    {
        prgBankMap[0] = r6;
        prgBankMap[1] = r7;
        prgBankMap[2] = secondLastBank;
        prgBankMap[3] = lastBank;
    }
    else
    {
        prgBankMap[0] = secondLastBank;
        prgBankMap[1] = r7;
        prgBankMap[2] = r6;
        prgBankMap[3] = lastBank;
    }
}
void MMC3::updateCHRBanks()
{
    int totalCHR = chr.size() / 0x400;
    if (totalCHR == 0)
        return;
    bankRegister[0] &= 0xFE;
    bankRegister[1] &= 0xFE;

    if (chrMode == 0)
    {
        chrBankMap[0] = bankRegister[0] % totalCHR;
        chrBankMap[1] = (bankRegister[0] + 1) % totalCHR;

        chrBankMap[2] = bankRegister[1] % totalCHR;
        chrBankMap[3] = (bankRegister[1] + 1) % totalCHR;

        chrBankMap[4] = bankRegister[2] % totalCHR;
        chrBankMap[5] = bankRegister[3] % totalCHR;
        chrBankMap[6] = bankRegister[4] % totalCHR;
        chrBankMap[7] = bankRegister[5] % totalCHR;
    }
    else
    {
        chrBankMap[4] = bankRegister[0] % totalCHR;
        chrBankMap[5] = (bankRegister[0] + 1) % totalCHR;

        chrBankMap[6] = bankRegister[1] % totalCHR;
        chrBankMap[7] = (bankRegister[1] + 1) % totalCHR;

        chrBankMap[0] = bankRegister[2] % totalCHR;
        chrBankMap[1] = bankRegister[3] % totalCHR;
        chrBankMap[2] = bankRegister[4] % totalCHR;
        chrBankMap[3] = bankRegister[5] % totalCHR;
    }
}
void MMC3::clockIRQ(int CurrentA12)
{
    if (prevA12 == 0 && CurrentA12 == 1)
    {
        if (irqCounter == 0 || irqReloadFlag)
        {
            irqCounter = irqReloadValue;
            irqReloadFlag = false;
        }
        else
        {
            irqCounter--;
        }

        if (irqCounter == 0 && irqEnabled)
            bus.cpu->setIRQ();
    }

    prevA12 = CurrentA12;
}
uint8_t MMC3::cpuRead(uint16_t addr)
{
    if (addr >= 0x6000 && addr <= 0x7FFF)
    {
        if (wramEnabled)
        {
            return wram[addr - 0x6000];
        }
        else
        {
            return 0;
        }
    }
    if (addr < 0x8000)
        return 0;
    if (addr >= 0x8000)
    {

        uint8_t slot = (addr - 0x8000) / 0x2000;
        int lastBank = (prg.size() / 0x2000) - 1;
        int secondLastBank = lastBank - 1;
        uint32_t bankNumber = prgBankMap[slot];
        uint32_t romIndex = bankNumber * 0x2000 + (addr & 0x1FFF);
        if (romIndex >= prg.size())
            return 0xFF;
        return prg[romIndex];
    }

    return 0xFF;
}
void MMC3::cpuWrite(uint16_t addr, uint8_t data)
{
    if (addr >= 0x6000 && addr <= 0x7FFF)
    {
        if (wramEnabled)
        {
            if (!wramWriteProtected)
            {
                wram[addr - 0x6000] = data;
            }
        }
    }
    else if (addr >= 0x8000 && addr <= 0x9FFF)
    {
        if ((addr & 1) == 0)
        {

            bankSelect = data & 0x07;
            prgMode = (data & 0x40) >> 6;
            chrMode = (data & 0x80) >> 7;
        }
        else
        {

            bankRegister[bankSelect] = data;

            if (bankSelect <= 5)
                updateCHRBanks();
            else
                updatePRGBanks();
        }
        return;
    }
    else if (addr >= 0xA000 && addr <= 0xBFFF)
    {
        if ((addr & 1) == 0)
        {
            mirrorHorizontal = !(data & 0x01);
        }
        else
        {
            wramEnabled = data & 0x80;
            wramWriteProtected = data & 0x40;
        }
        return;
    }
    else if (addr >= 0xC000 && addr <= 0xDFFF)
    {
        if ((addr & 1) == 0)
        {
            irqReloadValue = data;
        }
        else
        {
            irqReloadFlag = true;
        }
    }
    else if (addr >= 0xE000)
    {
        if ((addr & 1) == 0)
        {
            irqEnabled = false;
            bus.cpu->clearIRQ();
        }
        else
        {
            irqEnabled = true;
        }
    }
}
uint8_t MMC3::ppuRead(uint16_t addr)
{
    uint16_t original = addr;
    addr &= 0x3FFF;

    if (addr >= 0x0000 && addr <= 0x1FFF)
    {
        uint8_t slot = addr / 0x400;
        uint8_t bankNumber = chrBankMap[slot];
        int bankSize;
        if (slot <= 5)
            bankSize = 0x400;
        else
            bankSize = 0x800;
        uint32_t romIndex = bankNumber * bankSize + (addr % bankSize);
        if (romIndex >= chr.size())
            return 0xFF;
        clockIRQ((original & 0x1000) >> 12);
        return chr[romIndex];
    }
    else if (addr >= 0x2000 && addr <= 0x3EFF)
    {
        addr = addr % 0x1000;
        uint8_t ntIndex = addr / 0x400;
        uint8_t vramIndex;
        uint8_t offset = addr % 0x400;
        if (mirrorHorizontal)
        {
            if (ntIndex == 0 || ntIndex == 1)
                vramIndex = 0;
            else
                vramIndex = 1;
        }
        else
        {
            if (ntIndex == 0 || ntIndex == 2)
                vramIndex = 0;
            else
                vramIndex = 1;
        }
        clockIRQ((original & 0x1000) >> 12);
        return bus.ppu->vram[vramIndex * 0x400 + offset];
    }
    else if (addr >= 0x3F00 && addr <= 0x3FFF)
    {
        uint8_t palAddr = (addr - 0x3F00) & 0x1F;
        if (palAddr == 0x10)
            palAddr = 0x00;
        if (palAddr == 0x14)
            palAddr = 0x04;
        if (palAddr == 0x18)
            palAddr = 0x08;
        if (palAddr == 0x1C)
            palAddr = 0x0C;

        return bus.ppu->paletteRAM[palAddr];
    }
    return 0;
}
void MMC3::ppuWrite(uint16_t addr, uint8_t data)
{

    if (addr >= 0x0000 && addr <= 0x1FFF)
    {

        uint8_t slot = addr / 0x400;
        uint8_t bankNumber = chrBankMap[slot];
        int bankSize = (slot <= 5) ? 0x400 : 0x800;

        int index = bankNumber * bankSize + (addr % bankSize);
        chr[index] = data;

        clockIRQ((addr & 0x1000) >> 12);
    }

    else if (addr >= 0x2000 && addr <= 0x3EFF)
    {
        addr = addr % 0x1000;
        uint8_t ntIndex = addr / 0x400;
        uint8_t offset = addr % 0x400;
        uint8_t vramIndex;

        if (mirrorHorizontal)
        {
            vramIndex = (ntIndex < 2) ? 0 : 1;
        }
        else
        {
            vramIndex = (ntIndex % 2 == 0) ? 0 : 1;
        }
        clockIRQ((addr & 0x1000) >> 12);
        bus.ppu->vram[vramIndex * 0x400 + offset] = data;
    }

    else if (addr >= 0x3F00 && addr <= 0x3FFF)
    {
        uint8_t palAddr = (addr - 0x3F00) & 0x1F;
        if (palAddr == 0x10)
            palAddr = 0x00;
        if (palAddr == 0x14)
            palAddr = 0x04;
        if (palAddr == 0x18)
            palAddr = 0x08;
        if (palAddr == 0x1C)
            palAddr = 0x0C;

        bus.ppu->paletteRAM[palAddr] = data;
    }
}
Mirror MMC3::getMirror()
{
    return mirrorHorizontal ? HORIZONTAL : VERTICAL;
}
