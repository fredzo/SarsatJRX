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
        sprintf(buffer, "%f, %f", latitude.getFloatValue(),longitude.getFloatValue());
    }
    return buffer;
}

double Location::Angle::getFloatValue()
{
    double result = degrees;
    result += (((double)minutes)/60);
    result += (((double)seconds)/3600);
    return result;
}