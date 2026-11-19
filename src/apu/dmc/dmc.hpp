#include <cstdint>
#pragma once

class Bus;

class DMCChannel
{
public:
    bool enabled = false;

    bool irqEnabled = false;
    bool loop = false;
    bool irqPending = false;

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

    bool silence = true;
    bool dmaPending = false;
    uint8_t rateIndex = 0;
    uint8_t DACOutput = 0;

    void write(uint16_t address, uint8_t value);
    void clockTimer();
    void clockReader();
    void clockOutputUnit();
    void restartSample();
    void finishSample();
    void requestDMA();
    void onDMAComplete(uint8_t value);
    void setEnabled(bool enabled);

    uint8_t output(); // 7 bit

    Bus *bus = nullptr;
    void connectBus(Bus *bus);
};
