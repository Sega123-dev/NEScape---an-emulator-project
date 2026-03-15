#pragma once
#include <iostream>
#include <cstdint>
#include <vector>
#include "../mapper.hpp"
class Bus;
class NROM : public Mapper
{
    enum Mirroring
    {
        HORIZONTAL,
        VERTICAL
    };

private:
    Mirroring mirroring = HORIZONTAL;
    uint8_t lastCpuValue = 0;
    uint8_t lastPpuValue = 0;

public:
    std::vector<uint8_t> prg;
    std::vector<uint8_t> chr;
    std::vector<uint8_t> prgRam;
    bool prg16kb;
    bool chr8kb; // Checks if it's either CHR-RAM(0kb) or CHR-ROM(8kb)

    NROM(const std::vector<uint8_t> &prgData, const std::vector<uint8_t> &chrData, const bool &mirrorPortVertical);
    uint8_t cpuRead(uint16_t addr) override;
    uint8_t ppuRead(uint16_t addr) override;
    void ppuWrite(uint16_t addr, uint8_t data) override;
    void cpuWrite(uint16_t addr, uint8_t data) override;

    Mirror getMirror() override;
    void connectNrom(Bus *b);
    uint16_t mirror(uint16_t addr); // $2000-$2FFF
};