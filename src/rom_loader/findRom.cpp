#include "findRom.hpp"
#include <iostream>

namespace fs = std::filesystem;

fs::path findRom()
{
    fs::path cartridge = "cartridge";

    if (!fs::exists(cartridge))
        std::cout << "No folder found!" << std::endl;

    if (!fs::is_directory(cartridge))
        std::cout << "No dir!" << std::endl;

    fs::path romPath;
    int romCount = 0;

    for (const auto &entry : fs::directory_iterator(cartridge))
    {
        if (entry.is_regular_file() &&
            entry.path().extension() == ".nes")
        {
            romPath = entry.path();
            romCount++;
        }
    }

    if (romCount == 0)
    {
        return "default/empty_cartridge.nes";
    }

    if (romCount > 1)
    {
        std::cout << "Only one ROM in the cartridge is allowed!" << std::endl;
        return {};
    }

    return romPath;
}
