#include <cstdint>
#include <vector>
#include "pulse/pulse.hpp"
#include "traingle/traingle.hpp"
#include "noise/noise.hpp"
#include "dmc/dmc.hpp"
#include "frame_count/frame_counter.hpp"
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

    FrameCounter frameCounter;

    bool apuEnabled = true;
    bool pal = false;

    uint64_t cpuCycles = 0;
    bool cycleParity;

    double sampleRate = 48000;
    double sampleAccumulator = 0;
    double mixedOutput = 0.0f;

    std::vector<float> audioBuffer;

    void write(uint16_t addr, uint8_t value);
    uint8_t read(uint16_t addr);
    void clock();
    float mix();

    void generateSamples();
};