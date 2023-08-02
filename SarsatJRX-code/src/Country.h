#ifndef COUNTRY_H
#define COUNTRY_H

#include <Arduino.h>

class Country
{
    public:
        int code;
        String alphaCode;
        String longName;
        String shortName;
        void setValues(int code, String alphaCode, String longName, String shortName);
        String toString();
        static Country getCountry(int code);
};
#endif 