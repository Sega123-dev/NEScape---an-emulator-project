#pragma once

#include <SDL2/SDL.h>
#include <string>
#include <unordered_map>

#include "config_manager.hpp"
#include "controller.hpp"
#include "../bus/bus.hpp"

class InputManager
{
public:
    InputManager(Controller *p1, Controller *p2, ConfigManager *configManager);

    void handleEvent(const SDL_Event &event);

private:
    void processKey(SDL_Keycode key, bool pressed);

    // Sets a button on the stored pointer AND on the bus's controller
    void applyButton(Controller *own, Controller &busController,
                     const std::string &buttonName, bool pressed);

    Controller *player1;
    Controller *player2;
    ConfigManager *config;
};