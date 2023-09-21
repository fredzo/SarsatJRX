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

        static Settings *settingsInstance;
};

#endif 