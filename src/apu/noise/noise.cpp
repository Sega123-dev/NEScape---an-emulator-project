#include <cstdint>
#include "noise.hpp"

static constexpr uint8_t lengthTable[32] = {
    10, 254, 20, 2, 40, 4, 80, 6,
    160, 8, 60, 10, 14, 12, 26, 14,
    12, 16, 24, 18, 48, 20, 96, 22,
    192, 24, 72, 26, 16, 28, 32, 30};

static constexpr uint16_t noisePeriodTable[16] = {
    4, 8, 16, 32,
    64, 96, 128, 160,
    202, 254, 380, 508,
    762, 1016, 2034, 406};

void NoiseChannel::write(uint16_t addr, uint8_t value)
{
    switch (addr)
    {
    case 0x400C:
        lengthHalt = value & 0x20;
        constantVolume = value & 0x10;
        envelopeDivider = value & 0x0F;
        break;

    case 0x400D:
        break;

    case 0x400E:
        mode = (value & 0x80) != 0;
        timerPeriod = noisePeriodTable[value & 0x0F] / 2 - 1;
        break;

    case 0x400F:
        lengthCounter = lengthTable[(value >> 3) & 0x1F];
        envelopeRestart = true;
        break;
    }
}

void NoiseChannel::clockTimer()
{
    if (timerCounter == 0)
    {
        timerCounter = timerPeriod;
        clockLSFR();
    }
    else
        timerCounter--;
}

void NoiseChannel::clockLSFR()
{
    uint8_t bit0 = shiftRegister & 1;
    if (mode == 0)
        bit1 = (shiftRegister >> 1) & 1;
    else
        bit1 = (shiftRegister >> 6) & 1;

    feedback = bit0 ^ bit1;

    shiftRegister = (shiftRegister >> 1) | (feedback << 14);
}

void NoiseChannel::clockEnvelope()
{
    if (envelopeRestart)
    {
        envelopeRestart = false;
        envelopeDecay = 15;
        envelopeDividerCounter = envelopeDivider;
    }

    else
    {
        if (envelopeDividerCounter == 0)
        {
            envelopeDividerCounter = envelopeDivider;

            if (envelopeDecay > 0)
                envelopeDecay--;
            else if (lengthHalt)
                envelopeDecay = 15;
        }
        else
            envelopeDividerCounter--;
    }
}

void NoiseChannel::clockLengthCounter()
{
    if (lengthCounter > 0 && lengthHalt == false)
        lengthCounter--;
}

uint8_t NoiseChannel::output()
{
    if (lengthCounter == 0)
        return 0;

    if ((shiftRegister & 1) == 1)
        return 0;

    if (constantVolume)
        return envelopeDivider;

    else
        return envelopeDecay;
}