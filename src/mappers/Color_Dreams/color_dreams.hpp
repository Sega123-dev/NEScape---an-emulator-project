#pragma once
#include <cstdint>
#include <vector>
#include "../mapper.hpp"
class Bus;
class Color_Dreams : public Mapper
{

private:
    Mirror mirroring;
    uint8_t lastCpuValue = 0;
    uint8_t lastPpuValue = 0;

public:
    std::vector<uint8_t> prg;
    std::vector<uint8_t> chr;

    uint8_t prgBanks = 0;
    uint8_t chrBanks = 0;
    uint8_t currentPRGBank = 0;
    uint8_t currentCHRBank = 0;

    Color_Dreams(const std::vector<uint8_t> &prgData, const std::vector<uint8_t> &chrData);
    uint8_t cpuRead(uint16_t addr) override;
    uint8_t ppuRead(uint16_t addr) override;
    void ppuWrite(uint16_t addr, uint8_t data) override;
    void cpuWrite(uint16_t addr, uint8_t data) override;

    Mirror getMirror() override;
    void reset();
};