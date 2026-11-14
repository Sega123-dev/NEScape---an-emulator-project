#include <cstdint>
#pragma once

class PulseChannel
{
public:
    bool pulse1 = false;

    bool enabled = false;

    uint16_t timerPeriod = 0; // 11 bit
    uint16_t timerCounter = 0;

    uint8_t duty = 0;
    uint8_t sequencePosition = 0;

    uint8_t lengthCounter = 0;
    bool lengthHalt = false;

    bool constantVolume = false;
    bool envelopeStart = false;

    uint8_t envelopePeriod = 0;
    uint8_t envelopeDivider = 0;
    uint8_t envelopeDecay = 0;

    bool sweep = false;
    bool sweepNegate = false;
    bool sweepReload = false;

    uint8_t sweepPeriod = 0;
    uint8_t sweepDivider = 0;
    uint8_t sweepShift = 0;

    uint8_t outputVolume = 0;

    void write(uint16_t address, uint8_t value);
    void clockTimer();
    void clockEnvelope();
    void clockLengthCounter();
    void clockSweep();

    uint16_t sweepTarget();

    uint8_t output();
    uint8_t outputVolume();
};
