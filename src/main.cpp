#include "cpu/6502ricoh.hpp"
#include "bus/bus.hpp"
#include "mappers/MMC3/mmc3.hpp"
#include "mappers/iNES_reader/reader.hpp"
#include <iostream>
#include <SDL2/SDL.h>

int main(int argc, char *argv[])
{
    std::cout << "START\n";

    Bus nes;
    MMC3 mmc3;
    CPU cpu;
    PPU ppu;
    std::cout << "ROM DUMP\n";

    dumpROM("roms/smb3.nes");

    std::cout << "CONNECT\n";

    nes.mmc3 = &mmc3;
    nes.mapper = &mmc3;
    nes.cpu = &cpu;
    nes.ppu = &ppu;

    mmc3.connectBus(&nes);
    nes.cpu->connectBus(&nes);
    nes.ppu->connectBus(&nes);
    nes.cpu->reset();

    std::cout << "FRAME\n";

    nes.ppu->initWindow(); // Create SDL window

    while (true)
    {
        for (int i = 0; i < 29780; i++)
        {
            nes.ppu->clock();
            nes.ppu->clock();
            nes.ppu->clock();
            nes.cpu->clock();
        }

        nes.ppu->drawFrame();
    }
}
