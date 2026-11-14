#include "apu.hpp"
#include "../cpu/6502ricoh.hpp"

void APU::write(uint16_t addr, uint8_t value)
{

    switch (addr)
    {
    case 0x4000:
        pulse1.write(addr, value);
        break;
    case 0x4001:
        pulse1.write(addr, value);
        break;
    case 0x4002:
        pulse1.write(addr, value);
        break;
    case 0x4003:
        pulse1.write(addr, value);
        break;
    case 0x4004:
        pulse2.write(addr - 4, value);
        break;
    case 0x4005:
        pulse2.write(addr - 4, value);
        break;
    case 0x4006:
        pulse2.write(addr - 4, value);
        break;
    case 0x4007:
        pulse2.write(addr - 4, value);
        break;
    case 0x4015:
    {
        bool enabledPulse1 = pulse1.enabled = value & 0x01;
        bool enabledPulse2 = pulse2.enabled = value & 0x02;
        if (!enabledPulse1)
            pulse1.lengthCounter = 0;
        if (!enabledPulse2)
            pulse2.lengthCounter = 0;
        break;
    }
    case 0x4017:
    {
        fiveStepMode = (value & 0x80) != 0;
        IRQFire = (value & 0x40) != 0;

        if (IRQFire)
            frameIRQ = false;

        FCResetPending = true;
        FCResetDelay = (cpuCycles % 2 == 0) ? 3 : 4;
        break;
    }
    }
}

uint8_t APU::read(uint16_t addr)
{
    uint8_t status = 0;

    if (addr == 0x4015)
    {
        if (pulse1.lengthCounter > 0)
            status = status | 0x01;
        if (pulse2.lengthCounter > 0)
            status = status | 0x02;

        if (frameIRQ)
            status |= 0x40;

        frameIRQ = false;
    }

    return status;
}

void APU::clock()
{
    cpuCycles++;

    if (cpuCycles % 2 == 0)
    {
        pulse1.clockTimer();
        pulse2.clockTimer();
    }

    if (FCResetPending)
    {
        FCResetDelay--;

        if (FCResetDelay == 0)
        {
            FCResetPending = false;
            frameCounter = 0;

            if (fiveStepMode)
            {
                clockQuarterFrame();
                clockHalfFrame();
            }

            return;
        }
    }

    clockFrameCounter();
}

void APU::clockQuarterFrame()
{
    pulse1.clockEnvelope();
    pulse2.clockEnvelope();
}

void APU::clockHalfFrame()
{
    pulse1.clockLengthCounter();
    pulse2.clockLengthCounter();

    pulse1.clockSweep();
    pulse2.clockSweep();
}

void APU::clockFrameCounter()
{
    frameCounter++;

    if (fiveStepMode)
    {
        switch (frameCounter)
        {
        case 3729:
            clockQuarterFrame();
            break;
        case 7457:
            clockQuarterFrame();
            clockHalfFrame();
            break;
        case 11186:
            clockQuarterFrame();
            break;
        case 14915:
            // nothing
            break;
        case 18641:
            clockQuarterFrame();
            clockHalfFrame();
            frameCounter = 0;
            break;
        }
    }
    else
    {
        switch (frameCounter)
        {
        case 3729:
            clockQuarterFrame();
            break;
        case 7457:
            clockQuarterFrame();
            clockHalfFrame();
            break;
        case 11186:
            clockQuarterFrame();
            break;
        case 14915:
            clockQuarterFrame();
            clockHalfFrame();
            frameCounter = 0;
            break;
        }
    }
}