#pragma once
#include <cstdint>

enum Button
{
    START,
    SELECT,
    A,
    B,
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class Controller
{
public:
    void setButton(Button button, bool keyPress);
    void write(uint8_t data);
    uint8_t read();

private:
    uint8_t buttonsVal;
    uint8_t shiftRegister;
    uint8_t buttonIndex;
    bool strobe;
};