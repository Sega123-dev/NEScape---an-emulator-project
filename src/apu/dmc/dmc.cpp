#include <cstdint>
#include "dmc.hpp"
#include "../../bus/bus.hpp"

static constexpr uint16_t rates[16] = {
    428, 380, 340, 320,
    286, 254, 226, 214,
    190, 160, 142, 128,
    106, 85, 72, 54};

void DMCChannel::connectBus(Bus *bus)
{
    this->bus = bus;
}

void DMCChannel::write(uint16_t addr, uint8_t value)
{
    switch (addr)
    {
    case 0x4010:
        irqEnabled = value & 0x80;
        loop = value & 0x40;
        rateIndex = value & 0x0F;
        timerPeriod = rates[rateIndex];
        break;
    case 0x4011:
        DACOutput = value & 0x7F;
        break;
    case 0x4012:
        sampleAddress = 0xC000 + value * 64;
        break;
    case 0x4013:
        sampleLength = (value * 16) + 1;
        break;
    }
}

void DMCChannel::clockTimer()
{
    if (timerCounter == 0)
    {
        timerCounter = timerPeriod;
        clockOutputUnit();
    }
    else
        timerCounter--;
}

void DMCChannel::clockOutputUnit()
{
    if (!silence)
    {
        if (shiftRegister & 1)
        {
            if (DACOutput <= 125)
                DACOutput += 2;
        }
        else
        {
            if (DACOutput >= 2)
                DACOutput -= 2;
        }
    }

    shiftRegister >>= 1;

    bitsRemaining--;

    if (bitsRemaining == 0)
    {
        bitsRemaining = 8;

        if (sampleBufferEmpty)
            silence = true;
        else
        {
            silence = false;
            shiftRegister = sampleBuffer;
            sampleBufferEmpty = true;
        }
    }
}

void DMCChannel::clockReader()
{
    if (sampleBufferEmpty && bytesRemaining > 0 && !dmaPending)
    {
        requestDMA();
    }
}

void DMCChannel::restartSample()
{
    currentAddress = sampleAddress;
    bytesRemaining = sampleLength;
}

void DMCChannel::finishSample()
{
    if (loop)
        restartSample();
    else if (irqEnabled)
        irqPending = true;
}

void DMCChannel::setEnabled(bool enabled)
{
    this->enabled = enabled;

    if (!enabled)
        bytesRemaining = 0;
    else if (bytesRemaining == 0)
        restartSample();
}

void DMCChannel::requestDMA()
{
    if (sampleBufferEmpty && bytesRemaining > 0 && !dmaPending)
    {
        dmaPending = true;
        if (sampleBufferEmpty && bytesRemaining > 0)
            bus->cpu->requestDMC_DMA(currentAddress);
    }
}

void DMCChannel::onDMAComplete(uint8_t value)
{
    dmaPending = false;

    sampleBuffer = value;
    sampleBufferEmpty = false;

    if (currentAddress == 0xFFFF)
        currentAddress = 0x8000;
    else
        currentAddress++;

    bytesRemaining--;

    if (bytesRemaining == 0)
        finishSample();
}

uint8_t DMCChannel::output()
{
    return DACOutput;
}