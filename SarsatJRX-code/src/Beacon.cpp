#include "Beacon.h"

Beacon::Beacon(byte frameBuffer[])
{   // Get a local copy of the original frame
    memcpy(frame,frameBuffer,SIZE);
    parseFrame();
}

void Beacon::parseFrame()
{
    long latofmin, latofsec, lonofmin, lonofsec, protocolCode;
    bool protocolFlag, latoffset, lonoffset;

    longFrame = (frame[3] & 0x80) >> 7;      // frame size        bit 25
    protocolFlag = (frame[3] & 0x40) >> 6;   // protocol flag     bit 26
    protocolCode = (frame[4] & 0x0F);        // protocol code     bit 37-40
    unsigned long countryCode;
    countryCode = ((frame[3] & 0x3F) << 4 | (frame[4] & 0xF0) >> 4); // country code bit 27-36
    countryCode = countryCode & 0x3FF; // country code
    country = Country::getCountry(countryCode);
        
    if (frame[2] == 0xD0) 
    {  // Self-test message frame synchronisation byte
        frameMode = SELF_TEST;
    }
    else if (frame[2] == 0x2F) 
    { // Normal message frame synchronisation byte
        frameMode = NORMAL;
    }
    else{
        frameMode = UNKNOWN;
    }

    if (longFrame)
    {
        if(protocolFlag == 1) 
        {   // User loc protocol trame longue
            // TODO : Check if protocol code = 1,2,3,6,7
            location.latitude.orientation = (frame[13] & 0x10) >> 4;   
            location.latitude.degrees = ((frame[13] & 0x0F) << 3 | (frame[14] & 0xE0) >> 5);
            location.latitude.minutes = (frame[14] & 0x1E) >> 1;
            location.latitude.minutes = (location.latitude.minutes * 4);
            location.longitude.orientation = (frame[14] & 0x01);           
            location.longitude.degrees = (frame[15]);
            location.longitude.minutes = (frame[16] & 0xF0) >> 4;
            location.longitude.minutes = (location.longitude.minutes * 4);

            desciption = "User location Protocol";
        }
        else if (protocolFlag == 0 && (protocolCode == 8 /*ELT*/ || protocolCode == 10 /* EPIRB */ 
                || protocolCode == 11 /* PLB */ || protocolCode == 16 /* National Test Location Protocol */)) 
        { //Nat loc protocol / trame longue
                // TODO : ELT(DT) Location protocol => protocolCode == 9
                // TODO ? : RTS Location protocol => protocolCode == 13

            latoffset = (frame[14] & 0x80) >> 7;   // Latitude NAT LOCATION
            location.latitude.orientation = (frame[7] & 0x20) >> 5;   
            location.latitude.degrees = ((frame[7] & 0x1F) << 2 | (frame[8] & 0xC0) >> 6);
            location.latitude.minutes = (frame[8] & 0x3E) >> 1;
            location.latitude.minutes = (location.latitude.minutes * 2);
            latofmin = (frame[14] & 0x60) >> 5;
            latofsec = (frame[14] & 0x1E) >> 1;
            latofsec = (latofsec * 4);
            if (latoffset == 1) 
            {
                location.latitude.minutes += latofmin;
                location.latitude.seconds += latofsec;
            }
            else if (latoffset == 0) 
            {
                location.latitude.minutes -= latofmin;
                if (location.latitude.minutes < 0) 
                {
                    location.latitude.minutes += 60;
                    location.latitude.degrees -= 1;
                }
                location.latitude.seconds -= latofsec;
                if (location.latitude.seconds < 0) 
                {
                    location.latitude.seconds += 60;
                    location.latitude.minutes -= 1;
                }
            }

            lonoffset = (frame[14] & 0x01);       //Longitude NAT LOCATION
            location.longitude.orientation = (frame[8] & 0x01);           
            location.longitude.degrees = (frame[9]);
            location.longitude.minutes = (frame[10] & 0xF8) >> 3;
            location.longitude.minutes = (location.longitude.minutes * 2);
            lonofmin = (frame[15] & 0xC0) >> 6;
            lonofsec = (frame[15] & 0x3C) >> 2;
            lonofsec = (lonofsec * 4);
            if (lonoffset == 1) 
            {
                location.longitude.minutes += lonofmin;
                location.longitude.seconds += lonofsec;
            }
            else if (lonoffset == 0) 
            {
                location.longitude.minutes -= lonofmin;
                if (location.longitude.minutes < 0) 
                {
                    location.longitude.minutes += 60;
                    location.longitude.degrees -= 1;
                }
                location.longitude.seconds -= lonofsec;
                if (location.longitude.seconds < 0) 
                {
                    location.longitude.seconds += 60;
                    location.longitude.minutes -= 1;
                }
            }
            
            desciption = "National Location Protocol";
        }
        else if (protocolFlag == 0 && (protocolCode < 8 || protocolCode == 12 /* Ship Security Protocol */ || protocolCode == 14 /* Standard Test Location Prtocol */)) 
        {   //Std loc protocol trame longue
            // TODO : protocole code = 0 and 1 should be excluded
            latoffset = (frame[14] & 0x80) >> 7;  //Latitude STD LOCATION
            location.latitude.orientation = (frame[8] & 0x80) >> 7; 
            location.latitude.degrees = (frame[8] & 0x7F);
            location.latitude.minutes = (frame[9] & 0xC0) >> 6;
            location.latitude.minutes = (location.latitude.minutes * 15);
            latofmin = (frame[14] & 0x7C) >> 2;
            latofsec = ((frame[14] & 0x03) << 2 | (frame[15] & 0xC0) >> 6);
            latofsec = (latofsec * 4);
            if (latoffset == 1) 
            {
                location.latitude.minutes += latofmin;
                location.latitude.seconds += latofsec;
            }
            else if (latoffset == 0) 
            {
                location.latitude.minutes -= latofmin;
                if (location.latitude.minutes < 0) 
                {
                    location.latitude.minutes += 60;
                    location.latitude.degrees -= 1;
                }
                location.latitude.seconds -= latofsec;
                if (location.latitude.seconds < 0) 
                {
                    location.latitude.seconds += 60;
                    location.latitude.minutes -= 1;
                }
            }
            
            lonoffset = (frame[15] & 0x20) >> 5;  //Longitude STD LOCATION
            location.longitude.orientation = (frame[9] & 0x20) >> 5; 
            location.longitude.degrees = ((frame[9] & 0x1F) << 3 | (frame[10] & 0xE0) >> 5);
            location.longitude.minutes = (frame[10] & 0x18) >> 3;
            location.longitude.minutes = (location.longitude.minutes * 15);
            lonofmin = (frame[15] & 0x1F);
            lonofsec = (frame[16] & 0xF0) >> 4;
            lonofsec = (lonofsec * 4);
            if (lonoffset == 1) 
            {
                location.longitude.minutes += lonofmin;
                location.longitude.seconds += lonofsec;
            }
            else if (lonoffset == 0) 
            {
                location.longitude.minutes -= lonofmin;
                if (location.longitude.minutes < 0) 
                {
                    location.longitude.minutes += 60;
                    location.longitude.degrees -= 1;
                }
                location.longitude.seconds -= lonofsec;
                if (location.longitude.seconds < 0) 
                {
                    location.longitude.seconds += 60;
                    location.longitude.minutes -= 1;
                }
            }
            
            desciption = "Standard Location Protocol";
        }
        else
        { 
            desciption = "Unknown protocol";
        }
    }
    else { // User protocol short frame
        desciption = "User Protocol";
    }
}