#include "bus.hpp"
void Bus::write(uint16_t addr, uint8_t data)
{
    if (addr >= 0x0000 && addr <= 0x1FFF)
        ram[addr & 0x07FF] = data;
    else if (addr >= 0x2000 && addr <= 0x3FFF)
    {
        switch (addr & 0x7)
        {
        case 0:
            ppu->write2000(data);
            break;
        case 1:
            ppu->write2001(data);
            break;
        case 5:
            ppu->write2005(data);
            break;
        case 6:
            ppu->write2006(data);
            break;
        case 7:
            ppu->write2007(data);
            break;
        }
    }
    else if (addr >= 0x6000)
    {
        mapper->cpuWrite(addr, data);
    }
}
uint8_t Bus::read(uint16_t addr)
{
    if (!mapper)
    {
        std::cerr << "NULL MAPPER ACCESS\n";
        return 0xFF;
    }
    if (addr >= 0x0000 && addr <= 0x1FFF)
        return ram[addr & 0x07FF];
    else if (addr >= 0x2000 && addr <= 0x3FFF)
    {
        switch (addr & 0x7)
        {
        case 2:
            return ppu->read2002();
            break;
        case 7:
            return ppu->read2007();
            break;
        }
        return ppu->ppuReadRaw(addr & 0x7);
    }
    else if (addr >= 0x4020)
    {
        std::cout << "READ " << std::hex << addr << std::endl;
        if (!mapper)
        {
            std::cout << "MAPPER NULL\n";
            return 0xFF;
        }

        return mapper->cpuRead(addr);
    }
    return 0xFF;
}
