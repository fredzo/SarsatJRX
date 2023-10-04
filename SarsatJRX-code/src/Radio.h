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
        float radioFrequency; 

        // Static members
        static int power;
        static bool on;
        static DRA818::Parameters parameters;
        static float scanFrequency;
        static bool scanFreqBusy;
        static String version;

        static Radio *radioInstance;
};

#endif 