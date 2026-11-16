#include <cstdint>
#include "triangle.hpp"

static constexpr uint8_t triangleTable[32] = {
    15, 14, 13, 12, 11, 10, 9, 8,
    7, 6, 5, 4, 3, 2, 1, 0,
    0, 1, 2, 3, 4, 5, 6, 7,
    8, 9, 10, 11, 12, 13, 14, 15};

static constexpr uint8_t lengthTable[32] = {
    10, 254, 20, 2, 40, 4, 80, 6,
    160, 8, 60, 10, 14, 12, 26, 14,
    12, 16, 24, 18, 48, 20, 96, 22,
    192, 24, 72, 26, 16, 28, 32, 30};

void TriangleChannel::write(uint16_t addr, uint8_t value)
{
    switch (addr)
    {
    case 0x4008:
        controlFlag = (value & 0x80) != 0;
        linearCounterReload = value & 0x7F;
        break;
    case 0x4009:
        // unused
        break;
    case 0x400A:
        timerPeriod = (timerPeriod & 0x0700) | value;
        break;
    case 0x400B:
        timerPeriod = (timerPeriod & 0x00FF) | ((value & 0x07) << 8);
        if (enabled)
            lengthCounter = lengthTable[(value >> 3) & 0x1F];
        linearCounterReloadFlag = true;
        break;
    }
}

void TriangleChannel::clockLinearCounter()
{
    if (linearCounterReloadFlag)
        linearCounter = linearCounterReload;
    else if (linearCounter > 0)
        linearCounter--;

    if (!controlFlag)
        linearCounterReloadFlag = false;
}

void TriangleChannel::clockLengthCounter()
{
    if (enabled && lengthCounter > 0 && !controlFlag)
        lengthCounter--;
}
void TriangleChannel::clockTimer()
{
    if (timerCounter == 0)
    {
        timerCounter = timerPeriod;

        if (enabled && lengthCounter > 0 && linearCounter > 0)
            sequencePosition = (sequencePosition + 1) & 0x1F;
    }
    else
        timerCounter--;
}
uint8_t TriangleChannel::output()
{
    return triangleTable[sequencePosition];
}