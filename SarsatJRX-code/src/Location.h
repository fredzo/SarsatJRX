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
            Angle();
            Angle(long degrees);

            private :
            double floatValue = 255;
        };

        enum class LocationFormat { DECIMAL, SEXAGESIMAL, MAIDENHEAD_LOCATOR };

        Angle latitude = Angle(127);
        Angle longitude = Angle(255);
        void clear();
        boolean isUnknown();
        String toString(LocationFormat format);
        void formatFloatLocation(char* buffer, size_t size, const char* format);

        private :
        String decimalFormat;
        String sexagesimalFormat;
        String locatorFormat;

};
#endif 