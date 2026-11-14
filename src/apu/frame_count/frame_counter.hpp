#include <cstdint>
#pragma once

class FrameCounter
{
public:
    bool fiveStepMode = false;
    bool irqInhibit = false;
    bool irqFlag = false;

    uint32_t cycleCounter = 0;

    bool pendingWrite = false;
    uint8_t pendingValue = 0;
    uint8_t writeDelay = 0;

    void clockQuarterFrame();
    void clockHalfFrame();

    void clock();
    void write(uint8_t value);
};