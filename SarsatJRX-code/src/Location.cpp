#include "Location.h"

String Location::toString(bool sexagesimal)
{
    char buffer[32];
    if(sexagesimal)
    {   // Format :  <xxx°yy'zz"N, xxx°yy'zz"W>
        sprintf(buffer, "%ld°%02ld'%02ld\"%c, %ld°%02ld'%02ld\"%c", latitude.degrees,latitude.minutes, latitude.seconds, latitude.orientation ? 'S' : 'N',longitude.degrees, longitude.minutes, longitude.seconds, longitude.orientation ? 'W' : 'E');
    }
    else
    {   // Format :  <-xx.yyy, xxx.yyy>
        // sprintf %f is not supported by Arduino :-/
        char latStr[16];
        char longStr[16];
        /* 4 is mininum width, 2 is precision; float value is copied onto str_temp*/
        dtostrf(latitude.getFloatValue(), 6, 4, latStr);
        dtostrf(longitude.getFloatValue(), 6, 4, longStr);
        sprintf(buffer, "%s, %s",latStr,longStr);
    }
    return buffer;
}

void Location::clear()
{
    latitude.clear();
    longitude.clear();
}

void Location::Angle::clear()
{
    degrees = 0;
    minutes = 0;
    seconds = 0;
    orientation = false;
}

double Location::Angle::getFloatValue()
{
    double result = degrees;
    result += (((double)minutes)/60);
    result += (((double)seconds)/3600);
    if(orientation)
    {
        result = -result;
    }
    return result;
}