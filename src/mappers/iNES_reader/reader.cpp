#include "reader.hpp"
#include "../NROM/nrom.hpp"
#include "../CNROM/cnrom.hpp"
#include "../UxROM/uxrom.hpp"
#include "../MMC1/mmc1.hpp"
#include "../MMC3/mmc3.hpp"

#include "../../bus/bus.hpp"

#include <iostream>
#include <cstdint>
#include <fstream>
#include <vector>

uint8_t header[16];
int prgRomSize;
int chrRomSize;
uint8_t flag6;
uint8_t flag7;
int mapperID;
bool verticalMirroring;
bool SRAM;
bool fourScreenMirroring;
std::vector<uint8_t> PRG_ROM;
std::vector<uint8_t> CHR_ROM;

void dumpROM(const char *filename)
{
    std::vector<char> headerBuffer;
    std::vector<char> prgBuffer;
    std::vector<char> chrBuffer;
    std::ifstream rom(filename, std::ios::binary);
    headerBuffer.resize(16);
    if (rom.fail())
    {
        std::cerr << "Failed to run ROM(Error:NotFound)." << '\n';
        return;
    }
    if (rom.read(headerBuffer.data(), 16))
    {
        for (int i = 0; i < 16; i++)
        {
            header[i] = static_cast<uint8_t>(headerBuffer[i]);
        }
        std::cout << "Header parsed\n";
    }
    if (header[0] == 0x4E && header[1] == 0x45 && header[2] == 0x53 && header[3] == 0x1A)
    {
        flag6 = header[6];
        flag7 = header[7];
        prgRomSize = header[4] * 0x4000;
        chrRomSize = header[5] * 0x2000;

        prgBuffer.resize(prgRomSize);
        chrBuffer.resize(chrRomSize);

        verticalMirroring = (flag6 & 1);
        SRAM = (flag6 & 2);
        fourScreenMirroring = (flag6 & 8);

        mapperID = ((flag7 & 0xF0) | (flag6 >> 4));

        bool trainer = (flag6 & 4) != 0;
        if (trainer)
            rom.seekg(512, std::ios::cur);

        if (rom.read(prgBuffer.data(), prgRomSize))
        {
            PRG_ROM.assign(prgBuffer.begin(), prgBuffer.end());
        }
        std::cout << "PRG_ROM parsed\n";
        if (chrRomSize == 0)
        {
            CHR_ROM.resize(0x2000, 0); // 8 KB RAM
            std::cout << "Detected CHR RAM, allocating 8 KB\n";
        }
        else
        {
            chrBuffer.resize(chrRomSize);
            if (rom.read(chrBuffer.data(), chrRomSize))
            {
                CHR_ROM.assign(chrBuffer.begin(), chrBuffer.end());
                std::cout << "CHR_ROM parsed\n";
            }
        }
        rom.close();
        Mapper *mapper = chooseMapper();
        bus.mapper = chooseMapper();
        if (!bus.mapper)
            return;
    }
    else
    {
        std::cerr << "This file is not NES ROM file.Please provide a file that is using iNES format." << '\n';
    }
}

Mapper *chooseMapper()
{
    switch (mapperID)
    {
    case 0:
        std::cout << "NROM chosen" << std::endl;
        return new NROM(PRG_ROM, CHR_ROM, verticalMirroring);

    case 1:
        std::cout << "MMC1 chosen" << std::endl;
        return new MMC1(PRG_ROM, CHR_ROM, fourScreenMirroring, verticalMirroring);
    case 2:
        std::cout << "UXROM chosen" << std::endl;
        return new UXROM(PRG_ROM, CHR_ROM);
    case 3:
        std::cout << "CNROM chosen" << std::endl;
        return new CNROM(PRG_ROM, CHR_ROM);

    case 4:
        std::cout << "MMC3 chosen" << std::endl;
        return new MMC3(PRG_ROM, CHR_ROM, verticalMirroring);

    default:
        std::cerr << "NEScape does not support mapper " << mapperID << '\n';
        return nullptr;
    }
};
void connectReader(Bus *b)
{
    bus = *b;
}