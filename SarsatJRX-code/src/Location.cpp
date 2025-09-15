#include "Location.h"

#define GPS_LOCATOR_SIZE    10

bool Location::isUnknown()
{
    return (latitude.degrees >= 255 || longitude.degrees >= 255);
}

static char gps_letterize(int x) 
{
    return (char) x + 65;
}

static void gps_compute_locator(double lat, double lon, char* gps_locator) 
{
    double LON_F[]={20,2.0,0.083333,0.008333,0.0003472083333333333};
    double LAT_F[]={10,1.0,0.0416665,0.004166,0.0001735833333333333};
    int i;
    lon += 180;
    lat += 90;

    for (i = 0; i < GPS_LOCATOR_SIZE/2; i++)
    {
        if (i % 2 == 1) 
        {
            gps_locator[i*2] = (char) (lon/LON_F[i] + '0');
            gps_locator[i*2+1] = (char) (lat/LAT_F[i] + '0');
        }
        else 
        {
            gps_locator[i*2] = gps_letterize((int) (lon/LON_F[i]));
            gps_locator[i*2+1] = gps_letterize((int) (lat/LAT_F[i]));
        }
        lon = fmod(lon, LON_F[i]);
        lat = fmod(lat, LAT_F[i]);
    }
    gps_locator[i*2]=0;
}

String Location::toString(LocationFormat format)
{
    if(isUnknown())
    {
        return "GPS not synchronized";
    }
    switch(format)
    {
        case LocationFormat::SEXAGESIMAL:
            if(sexagesimalFormat.isEmpty())
            {   // Init value
                char buffer[32];
                // Format :  <xxx°yy'zz"N, xxx°yy'zz"W>
                snprintf(buffer,sizeof(buffer), "%ld°%02ld'%02ld\"%c, %ld°%02ld'%02ld\"%c", latitude.degrees,latitude.minutes, latitude.seconds, latitude.orientation ? 'S' : 'N',longitude.degrees, longitude.minutes, longitude.seconds, longitude.orientation ? 'W' : 'E');
                sexagesimalFormat = buffer;
            }
            return sexagesimalFormat;
            break;
        case LocationFormat::MAIDENHEAD_LOCATOR:
            if(locatorFormat.isEmpty())
            {   // Init value
                char buffer[32];
                // Locator format
                gps_compute_locator(latitude.getFloatValue(),longitude.getFloatValue(),buffer);
                locatorFormat = buffer;
            }
            return locatorFormat;
            break;
        default:
        case LocationFormat::DECIMAL:
            if(decimalFormat.isEmpty())
            {   // Init value
                char buffer[32];
                // Format :  <-xx.yyy, xxx.yyy>
                formatFloatLocation(buffer,sizeof(buffer),"%s, %s");
                decimalFormat = buffer;
            }
            return decimalFormat;
            break;
    }
}

void Location::formatFloatLocation(char* buffer, size_t size, const char* format)
{   // Format :  <-xx.yyy>
    // sprintf %f is not supported by Arduino :-/
    char latStr[16];
    latitude.toFloatString(latStr);
    char longStr[16];
    longitude.toFloatString(longStr);
    snprintf(buffer,size,format,latStr,longStr);
}

void Location::clear()
{
    latitude.clear();
    longitude.clear();
    decimalFormat = "";
    sexagesimalFormat = "";
    locatorFormat = "";
}

Location::Angle::Angle()
{
}

Location::Angle::Angle(long degrees)
{
    this->degrees = degrees;
}

void Location::Angle::clear()
{
    degrees = 255;
    minutes = 0;
    seconds = 0;
    orientation = false;
    floatValue = 255;
}

double Location::Angle::getFloatValue()
{
    if(floatValue >= 255)
    {   // Float value not computed yet
        floatValue = degrees;
        floatValue += (((double)minutes)/60);
        floatValue += (((double)seconds)/3600);
        if(orientation)
        {
            floatValue = -floatValue;
        }
    }
    return floatValue;
}

void Location::Angle::toFloatString(char* angleStr)
{   /* 6 is mininum width, 4 is precision; float value is copied onto angleStr */
    dtostrf(getFloatValue(), 6, 4, angleStr);
}