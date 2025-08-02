#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>
#include <Preferences.h>

#define FREQUENCY_COUNT     7

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

        uint8_t getDisplayBacklight();
        void setDisplayBacklight(uint8_t backlight);

        bool getScreenOffOnCharge();
        void setScreenOffOnCharge(bool active);

        bool getShowBatteryPercentage();
        void setShowBatteryPercentage(bool show);

        uint8_t getBuzzerLevel();
        void setBuzzerLevel(uint8_t volume);

        bool getFrameAnnoucementBuzzer();
        void setFrameAnnoucementBuzzer(bool active);

        void save();

    private :
        Preferences preferences;
        bool dirty = false; // True when save is needed
        float activeFrequencies[FREQUENCY_COUNT+1];

        Settings()
        {
        };

        ~Settings()
        {
        };

        static Settings *settingsInstance;
};

#endif 