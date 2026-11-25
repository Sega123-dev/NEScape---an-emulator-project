#pragma once
#include <SDL2/SDL.h>
#include <string>
#include <unordered_map>

struct InputConfig
{
    std::unordered_map<std::string, SDL_Keycode> player1;
    std::unordered_map<std::string, SDL_Keycode> player2;
};

struct GeneralConfig
{
    std::string language = "English";
    bool autoLoadLastROM = true;
    bool rememberLastDirectory = true;
    bool confirmExit = true;
};

struct InputSettings
{
    bool keyboardEnabled = true;
    bool gamepadEnabled = true;
    bool allowDuplicateBindings = false;
    bool ignoreInputWhenUnfocused = true;
};

struct VideoConfig
{
    int windowWidth = 1024;
    int windowHeight = 960;
    bool fullscreen = false;
    bool vSync = true;
    bool integerScaling = true;
    bool aspectRatioCorrection = true;
    bool crtFilter = false;
    bool showFPS = false;
};

struct AudioConfig
{
    bool enabled = true;
    int volume = 80;
    int sampleRate = 44100;
    int bufferSize = 1024;
};

struct EmulationConfig
{
    std::string region = "NTSC";
    bool pauseWhenUnfocused = false;
    bool speedLimit = true;
    int emulationSpeed = 100;
};

struct DebugConfig
{
    bool developerConsole = false;
    bool cpuLogging = false;
    bool ppuLogging = false;
    bool mapperLogging = false;
    bool showFrameTiming = false;
};

class ConfigManager
{
public:
    InputConfig inputConfig;
    GeneralConfig general;
    InputSettings input;
    VideoConfig video;
    AudioConfig audio;
    EmulationConfig emulation;
    DebugConfig debug;

    bool loadConfig(const std::string &filename);
};
