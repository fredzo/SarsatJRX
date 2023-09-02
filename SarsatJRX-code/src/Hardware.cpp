#include <Hardware.h>
#include <Power.h>

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

/**
 * @brief Returns actual VCC value
 * 
 * @return VCC value
 */
float Hardware::getVccValue()
{
    return getPowerVccValue();
}

/**
 * @brief Read VCC value into the provided char* with the format "x.xx"
 * 
 * @param vccString the char* to store the VCC string value in
 */
void Hardware::getVccStringValue(char* vccString)
{
    return getPowerVccStringValue(vccString);
}
