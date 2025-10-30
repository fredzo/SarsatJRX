#include <Hardware.h>
#include <Power.h>
#include <SarsatJRXConf.h>

Hardware *Hardware::hardware = nullptr;

void Hardware::bootScreen()
{
    display = new Display();
    display->bootScreen();
}

void Hardware::init()
{
    i2c = new I2CBus(Wire,I2C_SDA_PIN,I2C_SCL_PIN);
    power = Power::getPower();
    power->powerInit();
    rtc = Rtc::getRtc();
    rtc->rtcInit(i2c);
    filesystems = Filesystems::getFilesystems();
    filesystems->init();
    settings = Settings::getSettings();
    settings->init();
    if(!display) bootScreen();
    display->setup(i2c);
    audio = Audio::getAudio();
    audio->audioInit();
    soundManager = SoundManager::getSoundManager();
    soundManager->init();
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

Audio* Hardware::getAudio()
{
    return audio;
}

Power* Hardware::getPower()
{
    return power;
}

Filesystems* Hardware::getFilesystems()
{
   return filesystems;
}

SoundManager* Hardware::getSoundManager()
{
   return soundManager;
}
