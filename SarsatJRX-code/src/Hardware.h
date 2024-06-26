#pragma once

#include <i2c_bus.h>
#include <RTC.h>
#include <Display.h>
#include <Filesystems.h>
#include <Settings.h>
#include <Radio.h>

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

    void radioInit();

    Display* getDisplay();

    Rtc* getRtc();

    Filesystems* getFilesystems();

    Settings* getSettings();

    Radio* getRadio();

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
    Settings *settings = nullptr;
    Radio* radio = nullptr;

    static Hardware *hardware;

};




