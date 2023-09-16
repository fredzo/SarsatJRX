#pragma once

#include <i2c_bus.h>
#include <RTC.h>
#include <Display.h>
#include <Filesystems.h>

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

    Rtc* getRtc();

    Filesystems* getFilesystems();

    /**
     * @brief Returns actual VCC value
     * 
     * @return VCC value
     */
    float getVccValue();

    /**
     * @brief Read VCC value into the provided char* with the format "x.xx"
     * 
     * @return Rhe VCC string value
     */
    String getVccStringValue();

private:
    Hardware()
    {
    };

    ~Hardware()
    {
    };

    I2CBus *i2c = nullptr;
    Display *display = nullptr;
    Rtc *rtc = nullptr;
    Filesystems *filesystems = nullptr;

    static Hardware *hardware;

};




