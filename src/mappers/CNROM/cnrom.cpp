#include <iostream>
#include <cstdint>
#include <vector>
#include "cnrom.hpp"
#include "../../bus/bus.hpp"

void CNROM::reset()
{
    currentChrBank = 0;
}
CNROM::CNROM(std::vector<uint8_t> &prgData, std::vector<uint8_t> &chrData)
{
    prg = prgData;
    chr = chrData;

    chrBankCount = chr.size() / 0x2000;
    currentChrBank = 0;
}
uint8_t CNROM::cpuRead(uint16_t addr)
{
    uint8_t value;
    if (addr >= 0x8000 && addr <= 0xFFFF)
    {
        if (prg.size() == 0x4000)
        {
            value = prg[addr & 0x3FFF];
        }
        else
        {
            value = prg[addr & 0x7FFF];
        }
    }
    else
    {
        return lastCpuValue;
    }
    lastCpuValue = value;
    return value;
}
void CNROM::cpuWrite(uint16_t addr, uint8_t data)
{
    if (addr >= 0x8000)
        currentChrBank = (data & 0x03) % chrBankCount;
}
uint8_t CNROM::ppuRead(uint16_t addr)
{
    uint8_t value;
    if (addr < 0x2000)
    {
        uint16_t newAddr = addr + (currentChrBank * 0x2000);
        value = chr[newAddr];
    }
    else
    {
        value = lastPpuValue;
    }
    lastPpuValue = value;
    return value;
}
void CNROM::ppuWrite(uint16_t addr, uint8_t data)
{
    std::cout << "No ram writes for CNROM" << std::endl;
}
Mirror CNROM::getMirror() {}

void CNROM::connectBus(Bus *b)
{
    bus = *b;
}