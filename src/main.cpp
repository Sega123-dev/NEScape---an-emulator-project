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
    CPU cpu;
    PPU ppu;
    std::cout << "ROM DUMP\n";

    Mapper *mapper = dumpROM("roms/smb3.nes");

    if (!mapper)
        return -1;

    nes.mapper = mapper;
    nes.cpu = &cpu;
    nes.ppu = &ppu;

    nes.cpu->connectBus(&nes);
    nes.ppu->connectBus(&nes);

    nes.reset();
    nes.mapper->reset();
    nes.cpu->reset();

    nes.ppu->initMirror();

    std::cout << "FRAME\n";

    nes.ppu->initWindow(); // Create SDL window

    double cycleCounter = 0.0;
    const double CYCLES_PER_FRAME = 89342.0 / 3.0;

    while (true)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                return 0;
        }
        while (cycleCounter < CYCLES_PER_FRAME)
        {
            nes.ppu->clock();
            nes.ppu->clock();
            nes.ppu->clock();
            nes.cpu->clock();

            cycleCounter += 1.0;
        }

        nes.ppu->drawFrame();

        cycleCounter -= CYCLES_PER_FRAME;
    }
}
