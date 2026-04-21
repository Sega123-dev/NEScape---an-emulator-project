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

Mapper *dumpROM(const char *filename)
{
    std::vector<char> headerBuffer;
    std::vector<char> prgBuffer;
    std::vector<char> chrBuffer;
    std::ifstream rom(filename, std::ios::binary);

    if (rom.fail())
    {
        std::cerr << "Failed to open ROM\n";
        return nullptr;
    }

    headerBuffer.resize(16);

    if (!rom.read(headerBuffer.data(), 16))
    {
        std::cerr << "Failed to read header\n";
        return nullptr;
    }

    for (int i = 0; i < 16; i++)
        header[i] = static_cast<uint8_t>(headerBuffer[i]);

    std::cout << "Header parsed\n";

    if (!(header[0] == 0x4E && header[1] == 0x45 && header[2] == 0x53 && header[3] == 0x1A))
    {
        std::cerr << "Invalid NES ROM\n";
        return nullptr;
    }

    flag6 = header[6];
    flag7 = header[7];

    prgRomSize = header[4] * 0x4000;
    chrRomSize = header[5] * 0x2000;

    verticalMirroring = (flag6 & 1);
    SRAM = (flag6 & 2);
    fourScreenMirroring = (flag6 & 8);

    mapperID = ((flag7 & 0xF0) | (flag6 >> 4));

    bool trainer = (flag6 & 4) != 0;
    if (trainer)
        rom.seekg(512, std::ios::cur);

    std::cout << "Trainer present: " << trainer << "\n";
    prgBuffer.resize(prgRomSize);
    chrBuffer.resize(chrRomSize);

    rom.read(prgBuffer.data(), prgRomSize);
    PRG_ROM.assign(prgBuffer.begin(), prgBuffer.end());

    std::cout << "PRG first 4 bytes: "
              << std::hex
              << (int)PRG_ROM[0] << " "
              << (int)PRG_ROM[1] << " "
              << (int)PRG_ROM[2] << " "
              << (int)PRG_ROM[3] << "\n";
    std::cout << "PRG last 4 bytes: "
              << std::hex
              << (int)PRG_ROM[prgRomSize - 4] << " "
              << (int)PRG_ROM[prgRomSize - 3] << " "
              << (int)PRG_ROM[prgRomSize - 2] << " "
              << (int)PRG_ROM[prgRomSize - 1] << "\n";
    if (chrRomSize == 0)
    {
        CHR_ROM.resize(0x2000, 0);
        std::cout << "CHR RAM allocated\n";
    }
    else
    {
        rom.read(chrBuffer.data(), chrRomSize);
        CHR_ROM.assign(chrBuffer.begin(), chrBuffer.end());
        std::cout << "CHR_ROM parsed\n";
    }

    rom.close();

    std::cout << "Reader PRG size: " << PRG_ROM.size() << "\n";
    std::cout << std::hex;

    for (int i = 0; i < 16; i++)
    {
        std::cout << (int)PRG_ROM[PRG_ROM.size() - 16 + i] << " ";
    }

    Mapper *mapper = chooseMapper();

    if (!mapper)
    {
        std::cerr << "Mapper creation failed\n";
        return nullptr;
    }
    std::cout << "PRG blocks: " << (int)header[4] << "\n";
    std::cout << "CHR blocks: " << (int)header[5] << "\n";
    std::cout << "Mapper: " << mapperID << "\n";
    return mapper;
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
