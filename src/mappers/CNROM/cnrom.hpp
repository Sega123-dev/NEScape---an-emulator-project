#pragma once
#include <iostream>
#include <cstdint>
#include <vector>
#include <memory>
#include "../mapper.hpp"
class Bus;
class CNROM : public Mapper
{
public:
    CNROM(std::vector<uint8_t> &prgData, std::vector<uint8_t> &chrData);
    uint8_t cpuRead(uint16_t addr) override;
    uint8_t ppuRead(uint16_t addr) override;
    void cpuWrite(uint16_t addr, uint8_t data) override;
    void ppuWrite(uint16_t addr, uint8_t data) override;
    Mirror getMirror() override;
    void reset();

private:
    std::vector<uint8_t> prg;
    std::vector<uint8_t> chr;

    uint8_t currentChrBank = 0;
    uint8_t chrBankCount = 1;

    uint8_t lastCpuValue = 0;
    uint8_t lastPpuValue = 0;

    void connectBus(Bus *b);
};