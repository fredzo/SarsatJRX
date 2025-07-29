#pragma once

#include <i2c_bus.h>
#include <RTC.h>
#include <Display.h>
#include <Filesystems.h>
#include <Settings.h>
#include <Audio.h>
#include <Power.h>

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

    Settings* getSettings();

    Audio* getAudio();

    Power* getPower();

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
    Audio* audio = nullptr;
    Power* power = nullptr;

    static Hardware *hardware;

};




