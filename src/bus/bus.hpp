#pragma once
#include "../cpu/6502ricoh.hpp"
#include "../ppu/ricoh2c02ntsc.hpp"
#include "../mappers/mapper.hpp"
#include "../mappers/CNROM/cnrom.hpp"
#include "../mappers/iNES_reader/reader.hpp"
#include "../mappers/MMC1/mmc1.hpp"
#include "../mappers/MMC3/mmc3.hpp"
#include "../mappers/NROM/nrom.hpp"
#include "../mappers/UxROM/uxrom.hpp"

#include <cstdint>

class CPU;
class MMC3;
class PPU;
class Mapper;
class MMC1;
class NROM;
class UXROM;

class Bus
{
public:
    Bus() = default;
    void write(uint16_t addr, uint8_t data);
    uint8_t read(uint16_t addr);

    CPU cpu;
    MMC3 *mmc3;
    PPU *ppu;
    Mapper *mapper;
    CNROM *cnrom;
    MMC1 *mmc1;
    NROM *nrom;
    UXROM *uxrom;

    uint8_t ram[2048];
};