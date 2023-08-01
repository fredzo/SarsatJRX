#ifndef LOCATION_H
#define LOCATION_H

#include <Arduino.h>

class Location
{
    public:
        class Angle
        {
            public :
            long degrees = 0;
            long minutes = 0;
            long seconds = 0;
            bool orientation = false;
            double getFloatValue();
            void clear();
        };

        Angle longitude;
        Angle latitude;
        void clear();
        boolean isUnknown();
        String toString(bool sexagesimal);

};
#endif 