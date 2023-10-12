#include <Settings.h>
#include <SarsatJRXConf.h>

#define PREF_PREFIX         "SarsatJRX"
#define WIFI_STATE_KEY      "Wifi"
#define RADIO_STATE_KEY     "Radio"
#define RADIO_VOLUME_KEY    "Vol"
#define RADIO_FILTER1_KEY   "Filt1"
#define RADIO_FILTER2_KEY   "Filt2"
#define RADIO_FILTER3_KEY   "Filt3"

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

byte Settings::getRadioVolume()
{
    return preferences.getChar(RADIO_VOLUME_KEY,8);
}

void Settings::setRadioVolume(byte volume)
{
    preferences.putChar(RADIO_VOLUME_KEY,volume);
    dirty = true;
}

bool Settings::getRadioFilter1()
{
    return preferences.getBool(RADIO_FILTER1_KEY,true);
}

void Settings::setRadioFilter1(bool state)
{
    preferences.putBool(RADIO_FILTER1_KEY,state);
    dirty = true;
}

bool Settings::getRadioFilter2()
{
    return preferences.getBool(RADIO_FILTER2_KEY,true);
}

void Settings::setRadioFilter2(bool state)
{
    preferences.putBool(RADIO_FILTER2_KEY,state);
    dirty = true;
}

bool Settings::getRadioFilter3()
{
    return preferences.getBool(RADIO_FILTER3_KEY,true);
}

void Settings::setRadioFilter3(bool state)
{
    preferences.putBool(RADIO_FILTER3_KEY,state);
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
