#include <Settings.h>
#include <SarsatJRXConf.h>

#define PREF_PREFIX             "SarsatJRX"
#define WIFI_STATE_KEY          "Wifi"
#define DISPLAY_REVERSE         "DRev"
#define DISPLAY_BACKLIGHT       "DBL"
#define SHOW_BAT_PERCENTAGE     "SBP"
#define BUZZER_LEVEL            "BL"
#define FRAME_ANN_BUZZER        "FAB"
#define SCREEN_OFF_ON_CHRAGE    "SOOC"

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
    return preferences.getUChar(BUZZER_LEVEL,true);
}

void Settings::setBuzzerLevel(uint8_t level)
{
    preferences.putUChar(BUZZER_LEVEL,level);
    dirty = true;
}

bool Settings::getFrameAnnoucementBuzzer()
{
    return preferences.getBool(FRAME_ANN_BUZZER,true);
}

void Settings::setFrameAnnoucementBuzzer(bool active)
{
    preferences.putBool(FRAME_ANN_BUZZER,active);
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
