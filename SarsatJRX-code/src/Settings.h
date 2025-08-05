#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>
#include <Preferences.h>

class Settings
{
    public :
        static Settings *getSettings()
        {
            if (settingsInstance == nullptr) {
                settingsInstance = new Settings();
            }
            return settingsInstance;
        }

        void init();

        bool getWifiState();
        void setWifiState(bool state);

        bool getDisplayReverse();
        void setDisplayReverse(bool reverse);

        bool getScreenOffOnCharge();
        void setScreenOffOnCharge(bool active);

        bool getShowBatteryPercentage();
        void setShowBatteryPercentage(bool show);

        uint8_t getBuzzerLevel();
        void setBuzzerLevel(uint8_t volume);

        bool getTouchSound();
        void setTouchSound(bool active);

        bool getFrameSound();
        void setFrameSound(bool active);

        bool getCountDownSound();
        void setCountDownSound(bool active);

        bool getCountDownLeds();
        void setCountDownLeds(bool active);

        bool getReloadCountDown();
        void setReloadCountDown(bool active);

        void save();

    private :
        Preferences preferences;
        bool dirty = false; // True when save is needed

        Settings()
        {
        };

        ~Settings()
        {
        };

        bool wifiState;
        bool displayReverse;
        bool screenOffOnCharge;
        bool showBatteryPercentage;
        uint8_t buzzerLevel;
        bool touchSound;
        bool frameSound;
        bool countDownSound;
        bool countDownLeds;
        bool reloadCountDown;

        static Settings *settingsInstance;
};

#endif 