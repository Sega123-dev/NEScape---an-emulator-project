#pragma once
#include <iostream>
#include <cstdint>
#include <vector>
#include "../../bus/bus.hpp"
#include "../mapper.hpp"

class MMC3;
class MMC3 : public Mapper
{
public:
    MMC3() = default;
    MMC3(std::vector<uint8_t> &prgData, std::vector<uint8_t> &chrData, const bool &vertMirrorPort);

    uint8_t cpuRead(uint16_t addr) override;
    void cpuWrite(uint16_t addr, uint8_t data) override;

    uint8_t ppuRead(uint16_t addr) override;
    void ppuWrite(uint16_t addr, uint8_t data) override;

    Mirror getMirror() override;

    void updatePRGBanks();
    void updateCHRBanks();
    void clockIRQ(int currentA12);
    void reset();
    void connectBus(Bus *b);

private:
    std::vector<uint8_t> chr;
    std::vector<uint8_t> prg;
    std::vector<uint8_t> wram;

    uint8_t bankRegister[8];
    uint8_t bankSelect;
    bool prgMode;
    bool chrMode;

    int prgBankMap[4];
    int chrBankMap[8];
    bool mirrorHorizontal;

    bool wramEnabled;
    bool wramWriteProtected;

    uint8_t irqReloadValue;
    uint8_t irqCounter;
    bool irqEnabled;
    bool irqReloadFlag;
    bool prevA12;
};