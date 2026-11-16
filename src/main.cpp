#include "cpu/6502ricoh.hpp"
#include "bus/bus.hpp"
#include "mappers/MMC3/mmc3.hpp"
#include "mappers/iNES_reader/reader.hpp"
#include <iostream>
#include "apu/apu.hpp"
#include <SDL2/SDL.h>

int main(int argc, char *argv[])
{
    Bus nes;
    CPU cpu;
    PPU ppu;
    APU apu;
    Mapper *mapper = dumpROM("roms/mm2.nes");

    if (!mapper)
        return -1;

    nes.mapper = mapper;
    nes.cpu = &cpu;
    nes.ppu = &ppu;
    nes.apu = &apu;

    nes.cpu->connectBus(&nes);
    nes.ppu->connectBus(&nes);

    nes.reset();
    nes.mapper->reset();
    nes.cpu->reset();

    nes.ppu->initMirror();

    nes.ppu->initWindow(); // Create SDL window
    nes.apu->initAudio();  // Create SDL audio object

    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0)
    {
        std::cout << "SDL Audio error: " << SDL_GetError() << '\n';
        return -1;
    }

    if (!apu.initAudio())
    {
        std::cout << SDL_GetError() << '\n';
        return -1;
    }

    while (true)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                bus.apu->shutdownAudio();
                return 0;
            }
        }
        while (!nes.ppu->frameComplete)
        {

            nes.ppu->clock();
            nes.ppu->clock();
            nes.ppu->clock();
            nes.cpu->clock();
        }
        nes.ppu->frameComplete = false;

        nes.ppu->drawFrame();
    }
}
