#pragma once

#include <i2c_bus.h>
#include <RTC.h>
#include <Display.h>


class Hardware
{
public:
    static Hardware *getHardware()
    {
        if (hardware == nullptr) {
            hardware = new Hardware();
        }
        return hardware;
    }

    void init();

    Display* getDisplay();

    /**
     * @brief Returns actual VCC value
     * 
     * @return VCC value
     */
    float getVccValue()
    {
        return getVccValue();
    }

    /**
     * @brief Read VCC value into the provided char* with the format "x.xx"
     * 
     * @param vccString the char* to store the VCC string value in
     */
    void getVccStringValue(char* vccString)
    {
        return getVccStringValue(vccString);
    }

private:
    Hardware()
    {
    };

    ~Hardware()
    {
    };

    I2CBus *i2c = nullptr;
    Display *display = nullptr;

    static Hardware *hardware;

};




