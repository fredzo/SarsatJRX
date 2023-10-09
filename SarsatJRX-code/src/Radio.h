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
        bool isScanFrequencyBusy();
        bool isScanOn();
        int getPower();
        String getVersion();

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