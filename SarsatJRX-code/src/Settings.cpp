#include <Settings.h>
#include <SarsatJRXConf.h>

#define PREF_PREFIX         "SarsatJRX"
#define WIFI_STATE_KEY      "Wifi"
#define DISPLAY_REVERSE     "DRev"
#define DISPLAY_BACKLIGHT   "DBL"

Settings *Settings::settingsInstance = nullptr;

void Settings::init()
{
    preferences.begin(PREF_PREFIX, false);
}

bool Settings::getWifiState()
{
    return preferences.getBool(WIFI_STATE_KEY,true);
}

void Settings::setWifiState(bool state)
{
    preferences.putBool(WIFI_STATE_KEY,state);
    dirty = true;
}

bool Settings::getDisplayReverse()
{
    return preferences.getBool(DISPLAY_REVERSE,true);
}

void Settings::setDisplayBacklight(uint8_t backlight)
{
    preferences.putUChar(DISPLAY_BACKLIGHT,backlight);
    dirty = true;
}

uint8_t Settings::getDisplayBacklight()
{
    return preferences.getUChar(DISPLAY_BACKLIGHT,true);
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
