#include <cstdint>
#include <vector>
#include "pulse/pulse.hpp"
#include "traingle/traingle.hpp"
#include "noise/noise.hpp"
#include "dmc/dmc.hpp"
#pragma once
using namespace std;

class APU
{
public:
    PulseChannel pulse1;
    PulseChannel pulse2;
    TriangleChannel triangle;
    NoiseChannel noise;
    DMCChannel dmc;

    bool apuEnabled = true;
    bool pal = false;

    uint64_t cpuCycles = 0;

    uint32_t frameCounter;

    double sampleRate = 48000;
    double sampleAccumulator = 0;
    double mixedOutput = 0.0f;

    std::vector<float> audioBuffer;

    void write(uint16_t addr, uint8_t value);
    uint8_t read(uint16_t addr);
    void clock();
    float mix();

    void generateSamples();

    bool fiveStepMode = false;
    bool frameIRQ = false;
    bool IRQFire = false;

    bool FCResetPending = false;
    uint8_t FCResetDelay = 0;

    void clockFrameCounter();
    void clockQuarterFrame();
    void clockHalfFrame();
};