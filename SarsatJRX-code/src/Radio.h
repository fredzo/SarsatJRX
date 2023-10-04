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
        void setScanFrequencies(float* frequencies);
        void startScan();
        void stopScan();
        float getCurrentScanFrequency();
        bool isCurrentScanFrequencyBusy();
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

        HardwareSerial *radioSerial;
        DRA818 *dra;
        float *scanFrequencies = NULL;
        int currentScanFrequencyIndex = 0;
        bool scanOn = false;

        int power = 0;
        bool on = false;
        DRA818::Parameters parameters;
        float scanFrequency = 0;
        bool scanFreqBusy = false;
        String version;

        // Static members
        static Radio *radioInstance;
};

#endif 