#ifndef RADIO_H
#define RADIO_H

#include <Arduino.h>
#include <HardwareSerial.h>

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

    private :
        Radio()
        {
        };

        ~Radio()
        {
        };

        HardwareSerial radioSerial;

        static Radio *radioInstance;
};

#endif 