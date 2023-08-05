#ifndef BEACON_H
#define BEACON_H

#include <Arduino.h>
#include <Country.h>
#include <Location.h>
class Beacon
{
    public:
        enum FrameMode { NORMAL, SELF_TEST, UNKNOWN };
        static const byte SIZE = 18;
        bool longFrame;
        FrameMode frameMode;
        Country country;
        byte frame[SIZE];
        Location location;
        String desciption;
        uint64_t identifier;
        Beacon(byte frameBuffer[]);
    private:
        void parseFrame();
};
#endif 