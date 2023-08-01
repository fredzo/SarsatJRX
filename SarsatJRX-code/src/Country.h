#ifndef COUNTRY_H
#define COUNTRY_H

#include <Arduino.h>

class Country
{
    public:
        int code;
        const char* alphaCode;
        const char* longName;
        const char* shortName;
        void setValues(int code, const char* alphaCode, const char* longName, const char* shortName);
        String toString();
        static Country getCountry(int code);
};
#endif 