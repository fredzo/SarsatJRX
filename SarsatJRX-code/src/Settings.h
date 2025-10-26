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

        bool getBluetoothState();
        void setBluetoothState(bool state);

        bool getWifiState();
        void setWifiState(bool state);

        String getWifiSsid();
        void setWifiSsid(String ssid);

        String getWifiPassPhrase();
        void setWifiPassPhrase(String passPhrase);

        String getWifiSsid1();
        void setWifiSsid1(String ssid);

        String getWifiPassPhrase1();
        void setWifiPassPhrase1(String passPhrase);

        String getWifiSsid2();
        void setWifiSsid2(String ssid);

        String getWifiPassPhrase2();
        void setWifiPassPhrase2(String passPhrase);

        uint8_t getWifiNetworkNumber() { return 3;};
        String getWifiSsid(int index)
        {
            switch(index)
            {
                case 0:
                default:
                    return getWifiSsid();
                    break;
                case 1:
                    return getWifiSsid1();
                    break;
                case 2:
                    return getWifiSsid2();
                    break;
            }
        }
        String getWifiPassPhrase(int index)
        {
            switch(index)
            {
                case 0:
                default:
                    return getWifiPassPhrase();
                    break;
                case 1:
                    return getWifiPassPhrase1();
                    break;
                case 2:
                    return getWifiPassPhrase2();
                    break;
            }
        }

        String getTimeZone();
        void setTimeZone(String timeZone);

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

        String toKvpString();

        void setSettingValue(String key, String value, bool udpateApp);

        void handleTask();


    private :
        Preferences preferences;
        bool dirty = false; // True when save is needed
        bool dirtyApp = false; // True when app sync is needed

        Settings()
        {
        };

        ~Settings()
        {
        };

        void saveToConfigLines(std::vector<String>& lines, bool keepContent);
        void updateConfigLine(std::vector<String>& lines, bool keepContent, const Setting& setting, const String& value);

        bool bluetoothState;
        bool wifiState;
        String wifiSsid;
        String wifiPassPhrase;
        String wifiSsid1;
        String wifiPassPhrase1;
        String wifiSsid2;
        String wifiPassPhrase2;
        String timeZone;
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

        uint32_t lastSavedTime = 0;

        static Settings *settingsInstance;
};

#endif 