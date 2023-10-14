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

        class Frequency 
        {
            public :
                float value = -1;
                bool on = false;
        };

        void init();

        bool getWifiState();
        void setWifiState(bool state);
        bool getRadioState();
        void setRadioState(bool state);
        bool getRadioAutoVolume();
        void setRadioAutoVolume(bool state);
        byte getRadioVolume();
        void setRadioVolume(byte volume);
        bool getRadioFilter1();
        void setRadioFilter1(bool on);
        bool getRadioFilter2();
        void setRadioFilter2(bool on);
        bool getRadioFilter3();
        void setRadioFilter3(bool on);
        Frequency getFrequency(int index);
        void setFrequency(int index, Frequency frequency);
        void setFrequency(int index, float frequency);
        void setFrequencyOn(int index, bool on);
        int getFrequencyCount();
        float[] getActiveFrequencies();

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
        static Frequency NO_FREQUENCY;
        static float DEFAULT_FREQUENCIES[];
};

#endif 