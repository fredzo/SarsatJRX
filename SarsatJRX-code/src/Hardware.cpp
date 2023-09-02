#include "Hardware.h"

Hardware *Hardware::hardware = nullptr;

void Hardware::init()
{
    i2c = new I2CBus();
    rtcInit(i2c);
    display = new Display();
    display->setup(Display::Color::DarkGrey,i2c);
}

Display* Hardware::getDisplay()
{
    return display;
}