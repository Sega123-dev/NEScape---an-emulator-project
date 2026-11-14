#include <cstdint>
#pragma once

class NoiseChannel
{
public:
    bool enabled = false;

    uint16_t timerPeriod = 0;
    uint16_t timerCounter = 0;

    uint16_t shiftRegister = 1; // 15 bit
    bool mode = false;

    uint8_t lengthCounter = 0;
    bool lengthHalt = false;

    bool constantVolume = false;
    bool envelopeLoop = false;
    bool envelopeStart = false;

    uint8_t envelopePeriod = 0;
    uint8_t envelopeDivider = 0;
    uint8_t envelopeDecay = 0;

    void write(uint16_t address, uint8_t value);
    void clockTimer();
    void clockEnvelope();
    void clockLengthCounter();

    uint8_t output();
};