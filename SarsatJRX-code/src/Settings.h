#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>
#include <Preferences.h>
#include <vector>

class Settings
{
    public :
        class Setting
        {
            public :
            String key;
            String configKey;
            String desciption;
            Setting(const String keyParam, const String configKeyParam, const String desciptionParam) : key(keyParam), configKey(configKeyParam), desciption(desciptionParam) {};
        };

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

        bool getShowBatteryWarnMessage();
        void setShowBatteryWarnMessage(bool show);

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

        uint8_t getCountdownDuration();
        void setCountdownDuration(uint8_t duration);

        bool getAllowFrameSimu();
        void setAllowFrameSimu(bool active);

        bool getFilterOrbito();
        void setFilterOrbito(bool active);

        bool getFilterInvalid();
        void setFilterInvalid(bool active);

        void save();

        void saveToSd();

    private :
        Preferences preferences;
        bool dirty = false; // True when save is needed

        Settings()
        {
        };

        ~Settings()
        {
        };

        void saveToConfigLines(std::vector<String>& lines);
        void updateConfigLine(std::vector<String>& lines, const Setting& setting, const String& value);

        bool wifiState;
        bool displayReverse;
        bool screenOffOnCharge;
        bool showBatteryPercentage;
        bool showBatteryWarnMessage;
        uint8_t buzzerLevel;
        bool touchSound;
        bool frameSound;
        bool countDownSound;
        bool countDownLeds;
        bool reloadCountDown;
        uint8_t countdownDuration;
        bool allowFrameSimu;
        bool filterOrbito;
        bool filterInvalid;

        static Settings *settingsInstance;
};

#endif 