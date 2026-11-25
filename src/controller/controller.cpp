#include "controller.hpp"
#include <cstdint>

void Controller::setButton(Button button, bool keyPress)
{
    switch (button)
    {
    case A:
        keyPress ? buttonsVal |= 0x01 : buttonsVal &= ~0x01;
        break;

    case B:
        keyPress ? buttonsVal |= 0x02 : buttonsVal &= ~0x02;
        break;

    case SELECT:
        keyPress ? buttonsVal |= 0x04 : buttonsVal &= ~0x04;
        break;

    case START:
        keyPress ? buttonsVal |= 0x08 : buttonsVal &= ~0x08;
        break;

    case UP:
        keyPress ? buttonsVal |= 0x10 : buttonsVal &= ~0x10;
        break;

    case DOWN:
        keyPress ? buttonsVal |= 0x20 : buttonsVal &= ~0x20;
        break;

    case LEFT:
        keyPress ? buttonsVal |= 0x40 : buttonsVal &= ~0x40;
        break;

    case RIGHT:
        keyPress ? buttonsVal |= 0x80 : buttonsVal &= ~0x80;
        break;
    }
}

void Controller::write(uint8_t data)
{
    uint8_t prevVal = strobe;
    if (data & 0x01)
        strobe = true;

    else if (prevVal)
    {
        shiftRegister = buttonsVal;
        buttonIndex = 0;
        strobe = false;
    }
}

uint8_t Controller::read()
{
    if (strobe)
        return buttonsVal & 0x01;

    if (buttonIndex >= 8)
        return 0x01;

    uint8_t result = shiftRegister & 0x01;

    shiftRegister >>= 1;
    buttonIndex++;

    return result;
}