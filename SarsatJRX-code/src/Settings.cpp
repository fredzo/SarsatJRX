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
    // Load all settings on startup to avoid accessing them at runtime
    wifiState = preferences.getBool(WIFI_STATE_KEY,false);
    displayReverse = preferences.getBool(DISPLAY_REVERSE,false);
    screenOffOnCharge = preferences.getBool(SCREEN_OFF_ON_CHRAGE,true);
    showBatteryPercentage = preferences.getBool(SHOW_BAT_PERCENTAGE,true);
    buzzerLevel = preferences.getUChar(BUZZER_LEVEL,255);
    touchSound = preferences.getBool(TOUCH_SOUND,true);
    frameSound = preferences.getBool(FRAME_SOUND,true);
    countDownSound = preferences.getBool(COUNTDOWN_SOUND,true);
    countDownLeds = preferences.getBool(COUNTDOWN_LEDS,true);
    reloadCountDown = preferences.getBool(RELOAD_COUNTDOWN,false);
}

bool Settings::getWifiState()
{
    return wifiState;
}

void Settings::setWifiState(bool state)
{
    if(wifiState == state) return;
    wifiState = state;
    preferences.putBool(WIFI_STATE_KEY,state);
    dirty = true;
}

bool Settings::getDisplayReverse()
{
    return displayReverse;
}

void Settings::setDisplayReverse(bool state)
{
    if(displayReverse == state) return;
    displayReverse = state;
    preferences.putBool(DISPLAY_REVERSE,state);
    dirty = true;
}

bool Settings::getScreenOffOnCharge()
{
    return screenOffOnCharge;
}

void Settings::setScreenOffOnCharge(bool active)
{
    if(screenOffOnCharge == active) return;
    screenOffOnCharge = active;
    preferences.putBool(SCREEN_OFF_ON_CHRAGE,active);
    dirty = true;
}

bool Settings::getShowBatteryPercentage()
{
    return showBatteryPercentage;
}

void Settings::setShowBatteryPercentage(bool show)
{
    if(showBatteryPercentage == show) return;
    showBatteryPercentage = show;
    preferences.putBool(SHOW_BAT_PERCENTAGE,show);
    dirty = true;
}

uint8_t Settings::getBuzzerLevel()
{
    return buzzerLevel;
}

void Settings::setBuzzerLevel(uint8_t level)
{
    if(buzzerLevel == level) return;
    buzzerLevel = level;
    preferences.putUChar(BUZZER_LEVEL,level);
    dirty = true;
}

bool Settings::getTouchSound()
{
    return touchSound;
}

void Settings::setTouchSound(bool active)
{
    if(touchSound == active) return;
    touchSound = active;
    preferences.putBool(TOUCH_SOUND,active);
    dirty = true;
}

bool Settings::getFrameSound()
{
    return frameSound;
}

void Settings::setFrameSound(bool active)
{
    if(frameSound == active) return;
    frameSound = active;
    preferences.putBool(FRAME_SOUND,active);
    dirty = true;
}

bool Settings::getCountDownSound()
{
    return countDownSound;
}

void Settings::setCountDownSound(bool active)
{
    if(countDownSound == active) return;
    countDownSound = active;
    preferences.putBool(COUNTDOWN_SOUND,active);
    dirty = true;
}

bool Settings::getCountDownLeds()
{
    return countDownLeds;
}

void Settings::setCountDownLeds(bool active)
{
    if(countDownLeds == active) return;
    countDownLeds = active;
    preferences.putBool(COUNTDOWN_LEDS,active);
    dirty = true;
}

bool Settings::getReloadCountDown()
{
    return reloadCountDown;
}

void Settings::setReloadCountDown(bool active)
{
    if(reloadCountDown == active) return;
    reloadCountDown = active;
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
