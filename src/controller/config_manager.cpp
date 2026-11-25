
#include "config_manager.hpp"
#include "../../extern/ini.h"

#include <iostream>
#include <cstring>
#include <cstdlib>

static bool parseBool(const char *value)
{
    return strcmp(value, "true") == 0 ||
           strcmp(value, "True") == 0 ||
           strcmp(value, "1") == 0;
}

static int parseInt(const char *value)
{
    return std::atoi(value);
}

static int configHandler(
    void *user,
    const char *section,
    const char *name,
    const char *value)
{
    ConfigManager *config = static_cast<ConfigManager *>(user);

    // PLAYER 1
    if (strcmp(section, "Player1") == 0)
    {
        SDL_Keycode key = SDL_GetKeyFromName(value);

        if (key == SDLK_UNKNOWN)
        {
            std::cout << "Unknown Player 1 key: " << value << '\n';
            return 1;
        }

        config->inputConfig.player1[name] = key;
        return 1;
    }

    // PLAYER 2
    if (strcmp(section, "Player2") == 0)
    {
        SDL_Keycode key = SDL_GetKeyFromName(value);

        if (key == SDLK_UNKNOWN)
        {
            std::cout << "Unknown Player 2 key: " << value << '\n';
            return 1;
        }

        config->inputConfig.player2[name] = key;
        return 1;
    }

    // GENERAL
    if (strcmp(section, "General") == 0)
    {
        if (strcmp(name, "Language") == 0)
            config->general.language = value;
        else if (strcmp(name, "AutoLoadLastROM") == 0)
            config->general.autoLoadLastROM = parseBool(value);
        else if (strcmp(name, "RememberLastDirectory") == 0)
            config->general.rememberLastDirectory = parseBool(value);
        else if (strcmp(name, "ConfirmExit") == 0)
            config->general.confirmExit = parseBool(value);
    }

    // INPUT
    else if (strcmp(section, "Input") == 0)
    {
        if (strcmp(name, "KeyboardEnabled") == 0)
            config->input.keyboardEnabled = parseBool(value);
        else if (strcmp(name, "GamepadEnabled") == 0)
            config->input.gamepadEnabled = parseBool(value);
        else if (strcmp(name, "AllowDuplicateBindings") == 0)
            config->input.allowDuplicateBindings = parseBool(value);
        else if (strcmp(name, "IgnoreInputWhenUnfocused") == 0)
            config->input.ignoreInputWhenUnfocused = parseBool(value);
    }

    // VIDEO
    else if (strcmp(section, "Video") == 0)
    {
        if (strcmp(name, "WindowWidth") == 0)
            config->video.windowWidth = parseInt(value);
        else if (strcmp(name, "WindowHeight") == 0)
            config->video.windowHeight = parseInt(value);
        else if (strcmp(name, "Fullscreen") == 0)
            config->video.fullscreen = parseBool(value);
        else if (strcmp(name, "VSync") == 0)
            config->video.vSync = parseBool(value);
        else if (strcmp(name, "IntegerScaling") == 0)
            config->video.integerScaling = parseBool(value);
        else if (strcmp(name, "AspectRatioCorrection") == 0)
            config->video.aspectRatioCorrection = parseBool(value);
        else if (strcmp(name, "CRTFilter") == 0)
            config->video.crtFilter = parseBool(value);
        else if (strcmp(name, "ShowFPS") == 0)
            config->video.showFPS = parseBool(value);
    }

    // AUDIO
    else if (strcmp(section, "Audio") == 0)
    {
        if (strcmp(name, "Enabled") == 0)
            config->audio.enabled = parseBool(value);
        else if (strcmp(name, "Volume") == 0)
            config->audio.volume = parseInt(value);
        else if (strcmp(name, "SampleRate") == 0)
            config->audio.sampleRate = parseInt(value);
        else if (strcmp(name, "BufferSize") == 0)
            config->audio.bufferSize = parseInt(value);
    }

    // EMULATION
    else if (strcmp(section, "Emulation") == 0)
    {
        if (strcmp(name, "Region") == 0)
            config->emulation.region = value;
        else if (strcmp(name, "PauseWhenUnfocused") == 0)
            config->emulation.pauseWhenUnfocused = parseBool(value);
        else if (strcmp(name, "SpeedLimit") == 0)
            config->emulation.speedLimit = parseBool(value);
        else if (strcmp(name, "EmulationSpeed") == 0)
            config->emulation.emulationSpeed = parseInt(value);
    }

    // DEBUG
    else if (strcmp(section, "Debug") == 0)
    {
        if (strcmp(name, "DeveloperConsole") == 0)
            config->debug.developerConsole = parseBool(value);
        else if (strcmp(name, "CPULogging") == 0)
            config->debug.cpuLogging = parseBool(value);
        else if (strcmp(name, "PPULogging") == 0)
            config->debug.ppuLogging = parseBool(value);
        else if (strcmp(name, "MapperLogging") == 0)
            config->debug.mapperLogging = parseBool(value);
        else if (strcmp(name, "ShowFrameTiming") == 0)
            config->debug.showFrameTiming = parseBool(value);
    }

    return 1;
}

bool ConfigManager::loadConfig(const std::string &filename)
{
    int result = ini_parse(
        filename.c_str(),
        configHandler,
        this);

    if (result < 0)
    {
        std::cerr << "Could not open config file: "
                  << filename << '\n';
        return false;
    }

    if (result > 0)
    {
        std::cerr << "Config parsing failed at line: "
                  << result << '\n';
        return false;
    }

    std::cout << "NEScape configuration loaded successfully!\n";
    return true;
}
