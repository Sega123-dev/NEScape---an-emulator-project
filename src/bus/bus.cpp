#include "bus.hpp"
void Bus::write(uint16_t addr, uint8_t data)
{
    openBus = data;

    if (addr >= 0x0000 && addr <= 0x1FFF)
        ram[addr & 0x07FF] = data;

    else if (addr >= 0x2000 && addr <= 0x3FFF)
    {

        switch (addr & 0x0007)
        {
        case 0x0:
            bus.ppu->write2000(data);
            break;
        case 0x1:
            bus.ppu->write2001(data);
            break;
        case 0x3:
            bus.ppu->write2003(data);
            break;
        case 0x4:
            bus.ppu->write2004(data);
            break;
        case 0x5:
            bus.ppu->write2005(data);
            break;
        case 0x6:
            bus.ppu->write2006(data);
            break;
        case 0x7:
            bus.ppu->write2007(data);
            break;
        }
        return;
    }
    else if (addr >= 0x4000 && addr <= 0x401F)
    {
        if (addr == 0x4014)
        {
            uint16_t startAddr = static_cast<uint16_t>(data) << 8;

            for (int i = 0; i < 256; i++)
            {
                bus.ppu->oam[(bus.ppu->OAMADDR + i) & 0xFF] = bus.read(startAddr + i);
            }

            bus.cpu->cycles += 513; // DMA STALL
        }
        else
        {
            return; // Other I/O
        }
    }

    else if (addr >= 0x4020 && addr <= 0xFFFF)
        bus.mapper->cpuWrite(addr, data);
}
uint8_t Bus::read(uint16_t addr)
{
    uint8_t data = openBus;

    if (!bus.mapper)
    {
        std::cerr << "NULL MAPPER ACCESS\n";
        return openBus;
    }

    if (addr >= 0x0000 && addr <= 0x1FFF)
    {
        data = ram[addr & 0x07FF];
    }
    else if (addr >= 0x2000 && addr <= 0x3FFF)
    {
        switch (addr & 0x0007)
        {
        case 0x2:
            data = bus.ppu->read2002();
            break;
        case 0x7:
            data = bus.ppu->read2007();
            break;
        case 0x4:
            data = bus.ppu->read2004();
            break;
        default:
            data = openBus;
            break;
        }
    }
    else if (addr >= 0x4000 && addr <= 0x401F)
        data = openBus;
    else if (addr >= 0x4020 && addr <= 0xFFFF)
        data = bus.mapper->cpuRead(addr);
    openBus = data;
    return data;
}

void Bus::reset()
{
    for (int i = 0; i < 2048; i++)
    {
        ram[i] = 0;
    }
}
