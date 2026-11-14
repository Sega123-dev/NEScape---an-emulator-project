#include <cstdint>
#pragma once

class DMCChannel
{
public:
    bool enabled = false;

    bool irqEnabled = false;
    bool loop = false;
    bool irqFlag = false;

    uint16_t timerPeriod = 0;
    uint16_t timerCounter = 0;

    uint16_t sampleAddress = 0xC000;
    uint16_t currentAddress = 0xC000;

    uint16_t sampleLength = 0;
    uint16_t bytesRemaining = 0;

    uint8_t sampleBuffer = 0;
    bool sampleBufferEmpty = true;

    uint8_t shiftRegister = 0;
    uint8_t bitsRemaining = 8;
    uint8_t outputLevel = 0;

    bool silence = true;

    bool dmaRequested = false;

    void write(uint16_t address, uint8_t value);
    void clockTimer();
    void clockOutputUnit();
    void requestDMA();
    void onDMAComplete(uint8_t value);

    uint8_t output(); // 7 bit
};
