#include <cstdint>
#pragma once

class TriangleChannel
{
public:
    bool enabled = false;

    uint16_t timerPeriod = 0; // 11 bit
    uint16_t timerCounter = 0;

    uint8_t sequencePosition = 0;
    uint8_t outputValue = 0;

    uint8_t lengthCounter = 0;
    bool controlFlag = false;

    bool lengthHalt = false;

    uint8_t linearCounter = 0; // 7 bit
    uint8_t linearCounterReload = 0;
    bool linearCounterReloadFlag = false;

    void write(uint16_t address, uint8_t value);
    void clockTimer();
    void clockLengthCounter();
    void clockLinearCounter();

    uint8_t output();
};