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
    case 0x4008:
    case 0x4009:
    case 0x400A:
    case 0x400B:
        triangle.write(addr, value);
        break;
    case 0x400C:
    case 0x400D:
    case 0x400E:
    case 0x400F:
        noise.write(addr, value);
        break;
    case 0x4015:
    {
        bool enabledPulse1 = pulse1.enabled = value & 0x01;
        bool enabledPulse2 = pulse2.enabled = value & 0x02;
        bool enabledTriangle = triangle.enabled = value & 0x04;
        bool enabledNoise = noise.enabled = value & 0x08;

        if (!enabledPulse1)
            pulse1.lengthCounter = 0;

        if (!enabledPulse2)
            pulse2.lengthCounter = 0;

        if (!enabledTriangle)
            triangle.lengthCounter = 0;

        if (!enabledNoise)
            noise.lengthCounter = 0;

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
        if (triangle.lengthCounter > 0)
            status |= 0x04;
        if (noise.lengthCounter > 0)
            status |= 0x08;
        if (frameIRQ)
            status |= 0x40;

        frameIRQ = false;
    }

    return status;
}

void APU::clock()
{
    cpuCycles++;
    triangle.clockTimer();
    if (cpuCycles % 2 == 0)
    {
        pulse1.clockTimer();
        pulse2.clockTimer();

        noise.clockTimer();
    }

    bool resetOccurred = false;

    if (FCResetPending)
    {
        if (FCResetDelay > 0)
            FCResetDelay--;

        if (FCResetDelay == 0)
        {
            FCResetPending = false;
            frameCounter = 0;
            resetOccurred = true;

            if (fiveStepMode)
            {
                clockQuarterFrame();
                clockHalfFrame();
            }
        }
    }

    if (!resetOccurred)
        clockFrameCounter();

    generateSamples();
}

void APU::clockQuarterFrame()
{
    pulse1.clockEnvelope();
    pulse2.clockEnvelope();
    noise.clockEnvelope();

    triangle.clockLinearCounter();
}

void APU::clockHalfFrame()
{
    pulse1.clockLengthCounter();
    pulse2.clockLengthCounter();

    pulse1.clockSweep();
    pulse2.clockSweep();

    triangle.clockLengthCounter();
    noise.clockLengthCounter();
}

void APU::clockFrameCounter()
{
    frameCounter++;

    if (fiveStepMode)
    {
        switch (frameCounter)
        {
        case 7457:
            clockQuarterFrame();
            break;

        case 14913:
            clockQuarterFrame();
            clockHalfFrame();
            break;

        case 22371:
            clockQuarterFrame();
            break;

        case 37281:
            clockQuarterFrame();
            clockHalfFrame();
            break;

        case 37282:
            frameCounter = 0;
            break;
        }
    }
    else
    {
        switch (frameCounter)
        {
        case 7457:
            clockQuarterFrame();
            break;

        case 14913:
            clockQuarterFrame();
            clockHalfFrame();
            break;

        case 22371:
            clockQuarterFrame();
            break;

        case 29828:
        case 29829: // nothing
        case 29830:
            if (!IRQFire)
                frameIRQ = true;

            if (frameCounter == 29829)
            {
                clockQuarterFrame();
                clockHalfFrame();
            }

            if (frameCounter == 29830)
                frameCounter = 0;

            break;
        }
    }
}

double APU::pulseMixer()
{
    uint8_t p1 = pulse1.output();
    uint8_t p2 = pulse2.output();

    if (p1 + p2 == 0)
        return 0.0;

    return 95.88 / ((8128.0 / (p1 + p2)) + 100.0);
}

double APU::tndMixer()
{
    double triangleInput = triangle.output() / 8227.0;
    double noiseInput = noise.output() / 12241.0;

    double input = triangleInput + noiseInput;

    if (input == 0.0)
        return 0.0;

    return 159.79 / ((1.0 / input) + 100.0);
}

double APU::mix()
{
    return tndMixer() + pulseMixer();
}

const int CPU_FREQUENCY = 1789773;

void APU::generateSamples()
{
    sampleTimer += sampleRate;

    if (sampleTimer >= CPU_FREQUENCY)
    {
        sampleTimer -= CPU_FREQUENCY;

        std::lock_guard<std::mutex> lock(audioMutex);

        audioBuffer.push_back(static_cast<float>(mix()));
    }
}

// SDL AUDIO

void APU::audioCallback(void *userdata, Uint8 *stream, int len)
{
    APU *apu = static_cast<APU *>(userdata);

    float *output = reinterpret_cast<float *>(stream);
    int sampleCount = len / sizeof(float);

    std::lock_guard<std::mutex> lock(apu->audioMutex);

    for (int i = 0; i < sampleCount; i++)
    {
        if (!apu->audioBuffer.empty())
        {
            output[i] = apu->audioBuffer.front();
            apu->audioBuffer.erase(apu->audioBuffer.begin());
        }
        else
        {
            output[i] = 0.0f;
        }
    }
}

bool APU::initAudio()
{
    SDL_AudioSpec want{}, have{};

    want.freq = 44100;
    want.format = AUDIO_F32SYS;
    want.channels = 1;
    want.samples = 512;
    want.callback = APU::audioCallback;
    want.userdata = this;

    audioDevice = SDL_OpenAudioDevice(
        nullptr, 0, &want, &have, 0);

    if (audioDevice == 0)
        return false;

    SDL_PauseAudioDevice(audioDevice, 0);

    return true;
}

void APU::shutdownAudio()
{
    if (audioDevice != 0)
    {
        SDL_CloseAudioDevice(audioDevice);
        audioDevice = 0;
    }
}