#include "input_manager.hpp"

InputManager::InputManager(
    Controller *p1,
    Controller *p2,
    ConfigManager *configManager)
{
    player1 = p1;
    player2 = p2;
    config = configManager;
}

void InputManager::applyButton(Controller *own, Controller &busController,
                               const std::string &buttonName, bool pressed)
{
    Button button;

    if (buttonName == "A")
        button = Button::A;
    else if (buttonName == "B")
        button = Button::B;
    else if (buttonName == "Select")
        button = Button::SELECT;
    else if (buttonName == "Start")
        button = Button::START;
    else if (buttonName == "Up")
        button = Button::UP;
    else if (buttonName == "Down")
        button = Button::DOWN;
    else if (buttonName == "Left")
        button = Button::LEFT;
    else if (buttonName == "Right")
        button = Button::RIGHT;
    else
        return;

    // The controller you passed in from main
    if (own)
        own->setButton(button, pressed);

    // The controller the CPU actually reads from $4016/$4017
    if (own != &busController)
        busController.setButton(button, pressed);
}

void InputManager::processKey(SDL_Keycode key, bool pressed)
{
    for (const auto &[buttonName, keyCode] : config->inputConfig.player1)
    {
        if (keyCode == key)
            applyButton(player1, bus.controller1, buttonName, pressed);
    }

    for (const auto &[buttonName, keyCode] : config->inputConfig.player2)
    {
        if (keyCode == key)
            applyButton(player2, bus.controller2, buttonName, pressed);
    }
}

void InputManager::handleEvent(const SDL_Event &event)
{
    switch (event.type)
    {
    case SDL_KEYDOWN:
        if (!event.key.repeat)
            processKey(event.key.keysym.sym, true);
        break;

    case SDL_KEYUP:
        processKey(event.key.keysym.sym, false);
        break;
    }
}