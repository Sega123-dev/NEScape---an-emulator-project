#include "cpu/6502ricoh.hpp"
#include "bus/bus.hpp"
#include "mappers/MMC3/mmc3.hpp"
#include "mappers/iNES_reader/reader.hpp"
#include <iostream>
#include "apu/apu.hpp"
#include <SDL2/SDL.h>
#include <filesystem>
#include "rom_loader/findROM.hpp"
#include "controller/config_manager.hpp"
#include "controller/input_manager.hpp"
int main(int argc, char *argv[])
{
    Bus nes;
    CPU cpu;
    PPU ppu;
    APU apu;
    ConfigManager configManager;
    std::filesystem::path romPath = findRom();
    std::string path = romPath.string();
    if (romPath.empty())
        return -1;

    if (!configManager.loadConfig("src/config.ini"))
    {
        std::cerr << "Failed to load config!\n";
        return 1;
    }
    Controller controller1;
    Controller controller2;

    InputManager inputManager(
        &controller1,
        &controller2,
        &configManager);

    Mapper *mapper = dumpROM(path.c_str());
    if (!mapper)
        return -1;

    nes.mapper = mapper;
    nes.cpu = &cpu;
    nes.ppu = &ppu;
    nes.apu = &apu;
    apu.dmc.connectBus(&nes);

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
            inputManager.handleEvent(event);

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
