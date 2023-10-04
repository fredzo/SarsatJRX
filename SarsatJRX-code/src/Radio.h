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

        HardwareSerial *radioSerial;
        DRA818 *dra;
        float radioFrequency; 
        bool on = false;

        // Static members
        static int power;

        static Radio *radioInstance;
};

#endif 