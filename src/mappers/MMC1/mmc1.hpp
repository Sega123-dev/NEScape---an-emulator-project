#pragma once
#include <iostream>
#include <cstdint>
#include <vector>
#include "../mapper.hpp"

#include "../../bus/bus.hpp"

class MMC1 : public Mapper
{
public:
    MMC1(const std::vector<uint8_t> &prgData, const std::vector<uint8_t> &chrData, const bool &lowHighMirrorPort, const bool &mirrorPortVertical);
    uint8_t cpuRead(uint16_t addr) override;
    uint8_t ppuRead(uint16_t addr) override;
    void ppuWrite(uint16_t addr, uint8_t data) override;
    Mirror getMirror();
    void cpuWrite(uint16_t addr, uint8_t data) override;
    void reset();

private:
    std::vector<uint8_t> prg;
    std::vector<uint8_t> chr;

    uint8_t shiftReg = 0;
    uint8_t shiftCount = 0;

    uint8_t controlReg = 0x0C;
    uint8_t chrBank0 = 0, chrBank1 = 0;
    uint8_t prgBank = 0;

    Mirror mirroring = HORIZONTAL;

    uint8_t lastCpuValue;
    uint8_t lastPpuValue;

    void connectMMC1(Bus *b);
};
