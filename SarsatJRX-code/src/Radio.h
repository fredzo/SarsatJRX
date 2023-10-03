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
        int getPower();
        String getVersion();

    private :
        Radio()
        {
        };

        ~Radio()
        {
        };

        HardwareSerial *radioSerial;
        DRA818 *dra;
        float radioFrequency; 

        static Radio *radioInstance;
};

#endif 