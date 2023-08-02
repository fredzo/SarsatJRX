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
            void toFloatString(char* angleStr);
        };

        Angle longitude;
        Angle latitude;
        void clear();
        boolean isUnknown();
        String toString(bool sexagesimal);
        void formatFloatLocation(char* buffer, const char* format);

};
#endif 