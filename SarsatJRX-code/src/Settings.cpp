#include <Settings.h>
#include <SarsatJRXConf.h>

#define PREF_PREFIX         "SarsatJRX"
#define WIFI_STATE_KEY      "Wifi"
#define RADIO_STATE_KEY     "Radio"

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

bool Settings::getRadioState()
{
    return preferences.getBool(RADIO_STATE_KEY,true);
}

void Settings::setRadioState(bool state)
{
    preferences.putBool(RADIO_STATE_KEY,state);
    dirty = true;
}

void Settings::save()
{
    if(dirty)
    {
        preferences.end();
        preferences.begin(PREF_PREFIX, false);
        dirty = false;
    }
}
