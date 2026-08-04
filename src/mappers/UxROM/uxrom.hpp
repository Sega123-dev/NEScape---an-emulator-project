#pragma once
#include <iostream>
#include <cstdint>
#include <vector>
#include <memory>
#include "../mapper.hpp"
#include "../../bus/bus.hpp"
class UXROM : public Mapper
{
public:
    UXROM(std::vector<uint8_t> &prgData, std::vector<uint8_t> &chrData, const bool &vertMirroringPort);
    uint8_t cpuRead(uint16_t addr) override;
    uint8_t ppuRead(uint16_t addr) override;
    void cpuWrite(uint16_t addr, uint8_t data) override;
    void ppuWrite(uint16_t addr, uint8_t data) override;
    Mirror getMirror() override;
    void reset();

private:
    std::vector<uint8_t> prg;
    std::vector<uint8_t> chr;

    uint8_t currentPRGbank = 0;
    uint8_t lastCpuValue = 0;
    uint8_t lastPpuValue = 0;

    Mirror horizontalMirroring;

    void connectUxrom(Bus *b);
};