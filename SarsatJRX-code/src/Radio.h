#ifndef RADIO_H
#define RADIO_H

#include <Arduino.h>

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

        static Radio *radioInstance;
};

#endif 