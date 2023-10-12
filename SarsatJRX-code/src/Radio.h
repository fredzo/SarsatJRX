#ifndef RADIO_H
#define RADIO_H

#include <Arduino.h>
#include <HardwareSerial.h>
#include <DRA818.h>

class Radio
{
    public :
        static Radio *getRadio()
        {
            if (radioInstance == nullptr) {
                radioInstance = new Radio();
            }
            return radioInstance;
        }

        void radioInit();
        void radioInit(byte volume, bool filter1, bool filter2, bool filter3);
        void radioStop();
        /*
         * Set scan frequencies : last frequency in the array must be 0 to tell the array end   
        */
        void setScanFrequencies(const float* frequencies);
        void startScan();
        void stopScan();
        void toggleScan();
        float getFrequency();
        void setFrequency(float frequency);
        void nextFrequency();
        void previousFrequency();
        bool isScanFrequencyBusy();
        bool isRadioOn();
        bool isScanOn();
        int getPower();
        String getVersion();
        // Settings
        void setVolume(byte volume);
        byte getVolume();
        void setFilter1(bool on);
        bool getFilter1();
        void setFilter2(bool on);
        bool getFilter2();
        void setFilter3(bool on);
        bool getFilter3();

        // Radio task processing
        void handleTask();

    private :
        Radio()
        {
        };

        ~Radio()
        {
        };

        void scanNext();

        static void rssiCallback(int rssi);
        static void readGroupCallback(DRA818::Parameters parameters);
        static void groupCallback(int retCode);
        static void handshakeCallback(int retCode);
        static void scanCallback(int retCode);
        static void volumeCallback(int retCode);
        static void filtersCallback(int retCode);
        static void versionCallback(String version);
        static void tailCallback(int retCode);

        HardwareSerial *radioSerial = NULL;
        DRA818 *dra = NULL;
        const float *scanFrequencies = NULL;
        int currentScanFrequencyIndex = -1;
        bool scanOn = false;

        // Radio parameters
        byte volume = 8;
        bool filter1 = false;
        bool filter2 = false;
        bool filter3 = false;

        int power = 0;
        bool on = false;
        DRA818::Parameters parameters;
        float radioFrequency = 0;
        bool scanFreqBusy = false;
        String version;
        

        // Static members
        static Radio *radioInstance;
};

#endif 