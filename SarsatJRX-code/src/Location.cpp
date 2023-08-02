#include "Location.h"

bool Location::isUnknown()
{
    return (latitude.degrees == 127 || longitude.degrees == 255);
}

String Location::toString(bool sexagesimal)
{
    if(isUnknown())
    {
        return "GPS not synchronized";
    }
    char buffer[32];
    if(sexagesimal)
    {   // Format :  <xxx°yy'zz"N, xxx°yy'zz"W>
        sprintf(buffer, "%ld°%02ld'%02ld\"%c, %ld°%02ld'%02ld\"%c", latitude.degrees,latitude.minutes, latitude.seconds, latitude.orientation ? 'S' : 'N',longitude.degrees, longitude.minutes, longitude.seconds, longitude.orientation ? 'W' : 'E');
    }
    else
    {   // Format :  <-xx.yyy, xxx.yyy>
        formatFloatLocation(buffer, "%s, %s");
    }
    return buffer;
}

void Location::formatFloatLocation(char* buffer, const char* format)
{   // Format :  <-xx.yyy>
    // sprintf %f is not supported by Arduino :-/
    char latStr[16];
    latitude.toFloatString(latStr);
    char longStr[16];
    longitude.toFloatString(longStr);
    sprintf(buffer, format, latStr,longStr);
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

void Location::Angle::toFloatString(char* angleStr)
{   /* 6 is mininum width, 4 is precision; float value is copied onto angleStr */
    dtostrf(getFloatValue(), 6, 4, angleStr);
}