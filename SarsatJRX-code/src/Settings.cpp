#include <Settings.h>
#include <SarsatJRXConf.h>

#define PREF_PREFIX         "SarsatJRX"
#define WIFI_STATE_KEY      "Wifi"

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

void Settings::save()
{
    if(dirty)
    {
        preferences.end();
        preferences.begin(PREF_PREFIX, false);
        dirty = false;
    }
}
