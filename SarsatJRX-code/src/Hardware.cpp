#include <Hardware.h>
#include <Power.h>
#include <SarsatJRXConf.h>

Hardware *Hardware::hardware = nullptr;

void Hardware::init()
{
    i2c = new I2CBus(Wire,I2C_SDA_PIN,I2C_SCL_PIN);
    rtc = Rtc::getRtc();
    rtc->rtcInit(i2c);
    filesystems = Filesystems::getFilesystems();
    filesystems->init();
    settings = Settings::getSettings();
    settings->init();
    display = new Display();
    display->setup(i2c);
    radio = Radio::getRadio();
}

void Hardware::radioInit()
{
    if(settings->getRadioState())
    {   // Init radio if setting is on
        radio->radioInit(settings->getRadioAutoVolume(),settings->getRadioVolume(),settings->getRadioFilter1(),settings->getRadioFilter2(),settings->getRadioFilter3(),settings->getLastFrequency(),settings->getActiveFrequencies());
    }
}


Display* Hardware::getDisplay()
{
    return display;
}

Rtc* Hardware::getRtc()
{
    return rtc;
}

Settings* Hardware::getSettings()
{
    return settings;
}

Radio* Hardware::getRadio()
{
    return radio;
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
String Hardware::getVccStringValue()
{
    return getPowerVccStringValue();
}

 Filesystems* Hardware::getFilesystems()
 {
    return filesystems;
 }
