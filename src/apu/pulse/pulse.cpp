#include <cstdint>
#include "pulse.hpp"

// TABLES

static constexpr uint8_t dutyTable[4][8] = {
    {0, 1, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 0, 0, 0},
    {1, 0, 0, 1, 1, 1, 1, 1}};

static constexpr uint8_t lengthTable[32] = {
    10, 254, 20, 2, 40, 4, 80, 6,
    160, 8, 60, 10, 14, 12, 26, 14,
    12, 16, 24, 18, 48, 20, 96, 22,
    192, 24, 72, 26, 16, 28, 32, 30};

// MAIN

void PulseChannel::clockTimer()
{
    if (timerCounter == 0)
    {
        timerCounter = timerPeriod;
        sequencePosition++;
        if (sequencePosition > 7)
            sequencePosition = 0;
    }
    else
    {
        timerCounter--;
    }
}

uint8_t PulseChannel::output()
{
    uint16_t target = sweepTarget();
    uint8_t volume = outputVolume();
    if (!enabled || lengthCounter == 0)
        return 0;

    if (timerPeriod < 8)
        return 0;

    if (sweepShift != 0 && target > 0x7FF)
        return 0;

    return dutyTable[duty][sequencePosition] * volume;
}

void PulseChannel::write(uint16_t addr, uint8_t value)
{
    switch (addr)
    {
    case 0x4000:
        duty = (value >> 6) & 0x03;
        lengthHalt = (value >> 5) & 0x01;
        constantVolume = (value >> 4) & 0x01;
        envelopePeriod = value & 0x0F;
        break;
    case 0x4001:
        sweep = (value >> 7) & 0x01;
        sweepDivider = (value >> 4) & 0x07;
        sweepNegate = (value >> 3) & 0x01;
        sweepShift = value & 0x07;
        break;
    case 0x4002:
        timerPeriod = (timerPeriod & 0x0700) | value;
        break;
    case 0x4003:
        timerPeriod = (timerPeriod & 0x00FF) | ((value & 0x07) << 8);

        if (enabled)
            lengthCounter = lengthTable[(value >> 3) & 0x1F];

        sequencePosition = 0;
        envelopeStart = true;
        break;
    }
}

void PulseChannel::clockEnvelope()
{
    if (envelopeStart)
    {
        envelopeDecay = 15;
        envelopeDivider = envelopePeriod;
        envelopeStart = false;
    }
    else
    {
        if (envelopeDivider > 0)
        {
            envelopeDivider--;
        }
        else
        {
            envelopeDivider = envelopePeriod;
            if (envelopeDecay > 0)
                envelopeDecay--;
            else if (lengthHalt)
                envelopeDecay = 15;
        }
    }
}

uint8_t PulseChannel::outputVolume()
{
    return constantVolume ? envelopePeriod : envelopeDecay;
}

void PulseChannel::clockLengthCounter()
{
    if (enabled && lengthCounter > 0 && !lengthHalt)
    {
        lengthCounter--;
    }
}

uint16_t PulseChannel::sweepTarget()
{
    uint16_t change;
    uint16_t target;
    change = timerPeriod >> sweepShift;

    if (!sweepNegate)
        target = timerPeriod + change;
    else
    {
        if (pulse1)
            target = timerPeriod - change - 1;
        else
            target = timerPeriod - change;
    }

    return target;
}

void PulseChannel::clockSweep()
{
    if (sweepDivider == 0)
    {
        if (sweep && sweepShift != 0 && timerPeriod >= 8)
        {
            uint16_t target = sweepTarget();

            if (target <= 0x7FF)
                timerPeriod = target;
        }
    }
    if (sweepDivider == 0 || sweepReload)
    {
        sweepDivider = sweepPeriod;
        sweepReload = false;
    }

    else
        sweepDivider--;
}
