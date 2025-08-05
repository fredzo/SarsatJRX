#include <Settings.h>
#include <SarsatJRXConf.h>

#define PREF_PREFIX             "SarsatJRX"
#define WIFI_STATE_KEY          "Wifi"
#define DISPLAY_REVERSE         "DRev"
#define SHOW_BAT_PERCENTAGE     "SBP"
#define SCREEN_OFF_ON_CHRAGE    "SOOC"
#define BUZZER_LEVEL            "BL"
#define TOUCH_SOUND             "TS"
#define FRAME_SOUND             "FS"
#define COUNTDOWN_SOUND         "CDS"
#define COUNTDOWN_LEDS          "CDL"
#define RELOAD_COUNTDOWN        "RCD"

Settings *Settings::settingsInstance = nullptr;

void Settings::init()
{
    preferences.begin(PREF_PREFIX, false);
    // TODO : use cache to load all settings once and get the values from memory after init
}

bool Settings::getWifiState()
{
    return preferences.getBool(WIFI_STATE_KEY,false);
}

void Settings::setWifiState(bool state)
{
    preferences.putBool(WIFI_STATE_KEY,state);
    dirty = true;
}

bool Settings::getDisplayReverse()
{
    return preferences.getBool(DISPLAY_REVERSE,false);
}

void Settings::setDisplayReverse(bool state)
{
    preferences.putBool(DISPLAY_REVERSE,state);
    dirty = true;
}

bool Settings::getScreenOffOnCharge()
{
    return preferences.getBool(SCREEN_OFF_ON_CHRAGE,true);
}

void Settings::setScreenOffOnCharge(bool active)
{
    preferences.putBool(SCREEN_OFF_ON_CHRAGE,active);
    dirty = true;
}

bool Settings::getShowBatteryPercentage()
{
    return preferences.getBool(SHOW_BAT_PERCENTAGE,true);
}

void Settings::setShowBatteryPercentage(bool show)
{
    preferences.putBool(SHOW_BAT_PERCENTAGE,show);
    dirty = true;
}

uint8_t Settings::getBuzzerLevel()
{
    return preferences.getUChar(BUZZER_LEVEL,255);
}

void Settings::setBuzzerLevel(uint8_t level)
{
    preferences.putUChar(BUZZER_LEVEL,level);
    dirty = true;
}

bool Settings::getTouchSound()
{
    return preferences.getBool(TOUCH_SOUND,true);
}

void Settings::setTouchSound(bool active)
{
    preferences.putBool(TOUCH_SOUND,active);
    dirty = true;
}

bool Settings::getFrameSound()
{
    return preferences.getBool(FRAME_SOUND,true);
}

void Settings::setFrameSound(bool active)
{
    preferences.putBool(FRAME_SOUND,active);
    dirty = true;
}

bool Settings::getCountDownSound()
{
    return preferences.getBool(COUNTDOWN_SOUND,true);
}

void Settings::setCountDownSound(bool active)
{
    preferences.putBool(COUNTDOWN_SOUND,active);
    dirty = true;
}

bool Settings::getCountDownLeds()
{
    return preferences.getBool(COUNTDOWN_LEDS,true);
}

void Settings::setCountDownLeds(bool active)
{
    preferences.putBool(COUNTDOWN_LEDS,active);
    dirty = true;
}

bool Settings::getReloadCountDown()
{
    return preferences.getBool(RELOAD_COUNTDOWN,false);
}

void Settings::setReloadCountDown(bool active)
{
    preferences.putBool(RELOAD_COUNTDOWN,active);
    dirty = true;
}

void Settings::save()
{
    if(dirty)
    {
        /*Serial.println("Saving preferences...");
        Serial.printf("Wifi : %d\n",getWifiState());
        Serial.printf("Radio : %d\n",getRadioState());
        Serial.printf("Filter1 : %d\n",getRadioFilter1());*/
        preferences.end();
        preferences.begin(PREF_PREFIX, false);
        dirty = false;
    }
}
