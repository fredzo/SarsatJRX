#include "Beacon.h"

uint64_t getBits(byte *data, int startBit, int endBit)
{
    uint64_t result = 0;
    // 0 bases bit count
    startBit--;
    int numBits = endBit - startBit;

    // get a pointer to the starting byte...
    const byte *pData = &(data[startBit / 8]);
    byte b = *pData;

    // calculate the starting bit within that byte...
    int bitOffset = 7 - (startBit % 8);

    // iterate for the desired number of bits...
    for(int i = 0; i < numBits; ++i)
    {
        // make room for the next bit...
        result <<= 1;
        // copy the bit...
        result |= ((b >> bitOffset) & 0x01);
        // reached the end of the current byte?
        if (--bitOffset < 0)
        {
            b = *(++pData); // go to the next byte...
            bitOffset = 7; // restart at the first bit in that byte...
        }
    }

    // all done...
    return result;
}

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
    if(protocolFlag)
    {   // User protocol
        protocolCode = ((frame[4] & 0x0E)) >> 1;  // protocol code     bit 37-39
    }
    else
    {   // Other protocols
        protocolCode = (frame[4] & 0x0F);    // protocol code     bit 37-40
    }
    unsigned long countryCode = getBits(frame, 27,36); // country code bit 27-36
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

    // Hex id from bits 26-85
    identifier = getBits(frame,26,85);

    if (longFrame)
    {
        if(protocolFlag == 1) 
        {   // User loc protocol trame longue
            location.latitude.orientation = (frame[13] & 0x10) >> 4;   
            location.latitude.degrees = ((frame[13] & 0x0F) << 3 | (frame[14] & 0xE0) >> 5);
            location.latitude.minutes = (frame[14] & 0x1E) >> 1;
            location.latitude.minutes = (location.latitude.minutes * 4);
            location.longitude.orientation = (frame[14] & 0x01);           
            location.longitude.degrees = (frame[15]);
            location.longitude.minutes = (frame[16] & 0xF0) >> 4;
            location.longitude.minutes = (location.longitude.minutes * 4);

            switch(protocolCode)
            {
                case 0b000 :
                    desciption = F("User loc. Prot. - Orbitography");
                    break;
                case 0b001 :
                    desciption = F("ELT - Aviation User Protocol");
                    break;
                case 0b010 :
                    desciption = F("EPIRB-Maritime User Protocol");
                    break;
                case 0b011 :
                    desciption = F("Serial User Location Protocol");
                    break;
                case 0b100 :
                    desciption = F("National User Loc. Protocol");
                    break;
                case 0b101 :
                    desciption = F("Second Generation Beacon");
                    break;
                case 0b110 :
                    desciption = F("EPIRB-Radio Call Sign User P.");
                    break;
                case 0b111 :
                    desciption = F("Test User Location Protocol");
                    break;
                default:
                    desciption = "Unknown Protocol (";
                    desciption += protocolCode;
                    desciption += ")";
            }
            // Identification data in bits 40-83
        }
        else // if (protocolFlag == 0)
        {   // Location decoding according to protocol code
            if(protocolCode == 0b1000 /*ELT*/ || protocolCode == 0b1010 /* EPIRB */ 
                || protocolCode == 0b1011 /* PLB */ || protocolCode == 0b1111 /* National Test Location Protocol */)
            {   // Nat loc protocol
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

                lonoffset = (frame[14] & 0x01);
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
                // Default location on bits 1/7-5/1/8-5 (27bits)
                identifier &= 0xFFFFFFFFF8000000ULL;
                identifier |= 0b011111110000001111111100000;
            }
            else if ((protocolCode > 1 && protocolCode < 8 )|| protocolCode == 0b1100 /* Ship Security Protocol */ || protocolCode == 0b1110 /* Standard Test Location Prtocol */)
            {   //Std loc protocol
                latoffset = (frame[14] & 0x80) >> 7;
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
                // Default location on bits 1/9/1/10
                identifier &= 0xFFFFFFFFFFE00000ULL;
                identifier |= 0b011111111101111111111;
            }
            else if((protocolCode == 0b1101) || (protocolCode == 0b1001))
            {   // RLS Location protocol => protocolCode == 0b1101 / ELT(DT) Location protocol => protocolCode == 0b1001
                latoffset = (frame[14] & 0x20) >> 5;
                location.latitude.orientation = (frame[8] & 0x20) >> 5; 
                location.latitude.degrees = ((frame[8] & 0x1F) << 2) | ((frame[9] & 0xC0) >> 6);
                location.latitude.minutes = (frame[9] & 0x20) >> 5;
                location.latitude.minutes = (location.latitude.minutes * 30);
                latofmin = (frame[14] & 0x1E) >> 1;
                latofsec = ((frame[14] & 0x01) << 3 | (frame[15] & 0xE0) >> 5);
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
                
                lonoffset = (frame[15] & 0x10) >> 4;
                location.longitude.orientation = (frame[9] & 0x10) >> 4; 
                location.longitude.degrees = ((frame[9] & 0x0F) << 4 | (frame[10] & 0xF0) >> 4);
                location.longitude.minutes = (frame[10] & 0x08) >> 3;
                location.longitude.minutes = (location.longitude.minutes * 30);
                lonofmin = (frame[15] & 0x0F);
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
                // Default location on bits 1/8/1/9 (19bits)
                identifier &= 0xFFFFFFFFFFF80000ULL;
                identifier |= 0b0111111110111111111;
            }
            else
            {   // Protocole code 0000 and 0001 = Spare : no location
            }
            switch(protocolCode)
            {
                case 0b0000 :
                case 0b0001 :
                    desciption = F("Spare Location Protocol");
                    break;
                case 0b0010 :
                    desciption = F("EPIRB - MMSI/Location Protocol");
                    break;
                case 0b0011 :
                    desciption = F("ELT-24-bit Address/Loc. Prot.");
                    break;
                case 0b0100 :
                case 0b0101 :
                    desciption = F("Serial Location Prot. - ELT");
                    break;
                case 0b0110 :
                    desciption = F("Serial Location Prot. - EPIRB");
                    break;
                case 0b0111 :
                    desciption = F("Serial Location Prot. - PLB");
                    break;
                case 0b1000 :
                    desciption = F("National Location Prot. - ELT");
                    break;
                case 0b1001 :
                    desciption = F("ELT(DT) Location Protocol");
                    break;
                case 0b1010 :
                    desciption = F("National Location Prot.-EPIRB");
                    break;
                case 0b1011 :
                    desciption = F("National Location Prot. - PLB");
                    break;
                case 0b1100 :
                    desciption = F("Ship Security");
                    break;
                case 0b1101 :
                    desciption = F("RLS Location Protocol");
                    break;
                case 0b1110 :
                    desciption = F("Standard Test Location Prot.");
                    break;
                case 0b1111 :
                    desciption = F("National Test Location Prot.");
                    break;
                default:
                    desciption = "Unknown Protocol (";
                    desciption += protocolCode;
                    desciption += ")";
            }
        }
    }
    else 
    {   // Short frame => User Protocol
        desciption = F("User Protocol - ");
        switch(protocolCode)
        {
            case 0b000 :
                desciption += F("Orbitography");
                break;
            case 0b001 :
                desciption += F("Aviation");
                break;
            case 0b010 :
                desciption += F("Maritime");
                break;
            case 0b011 :
                desciption += F("Serial");
                break;
            case 0b100 :
                desciption += F("National");
                break;
            case 0b101 :
                desciption += F("Spare");
                break;
            case 0b110 :
                desciption += F("Radio callsign");
                break;
            case 0b111 :
                desciption += F("Test");
                break;
            default:
                desciption += "Unknown (";
                desciption += protocolCode;
                desciption += ")";
        }
        // Identification data in bits 40-83
    }
}