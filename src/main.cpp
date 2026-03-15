#include "cpu/6502ricoh.hpp"
#include "bus/bus.hpp"
#include "mappers/MMC3/mmc3.hpp"
#include "mappers/iNES_reader/reader.hpp"
#include <iostream>
#include <SDL.h>

int main()
{
    Bus nes;
    MMC3 mmc3;
    dumpROM("roms/castlevania.nes");

    nes.mmc3 = &mmc3;
    mmc3.connectBus(&nes);
    nes.cpu->connectBus(&nes);
    nes.ppu->connectBus(&nes);
    nes.cpu->reset();

    while (true)
    {
        for (int i = 0; i < 29780; i++)
        {
            nes.cpu->clock();
            nes.ppu->clock();
            nes.ppu->clock();
            nes.ppu->clock();
        }

        nes.ppu->drawFrame();
    }
}
