#include <Filesystems.h>
#include <SarsatJRXConf.h>
#include <SPIFFS.h>
#include <Util.h>

void Filesystems::init()
{   // SPIFSS filesystem
    // Start SPIFSS
    if (SPIFFS.begin())
    {
        spiFilesystemMounted = true;
    }
    else
    {
        #ifdef SERIAL_OUT
        Serial.println("Could not mount SPIFFS");
        #endif
        spiFilesystemMounted = false;
    }
    // Start SD card
    SPIClass* sdhander = new SPIClass(HSPI);
    pinMode(SD_MISO, INPUT_PULLUP);
    sdhander->begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);
    if(SD.begin(SD_CS, *sdhander))
    {
        sdFilesystemMounted = true;
        // Create SarsatJRX folder if needed
        if(SD.mkdir(SARSATJRX_LOG_DIR))
        {
            logDirReady = true;
        }
        else
        {
            #ifdef SERIAL_OUT
            Serial.println("Could not create log directory.");
            #endif
        }
    }
    else
    {
        #ifdef SERIAL_OUT
        Serial.println("Could not  mount SD Card");
        #endif
        sdFilesystemMounted = false;
    }
}

bool Filesystems::saveBeacon(Beacon* beacon)
{
    bool result = false;
    if(sdFilesystemMounted&&logDirReady)
    {
        char buffer[64];
        sprintf(buffer,LOG_FILENAME_TEMPLATE,SARSATJRX_LOG_DIR,beacon->date.day,beacon->date.month,beacon->date.year,beacon->date.hour,beacon->date.minute,beacon->date.second);
        File file = sdFileSystem->open(buffer, FILE_WRITE);
        if(file)
        {
            if(!file.print(toHexString(beacon->frame,false,0,beacon->longFrame ? 18 : 14)))
            {   // SD card has probably been removed
                sdFilesystemMounted = false;
                #ifdef SERIAL_OUT
                Serial.println("Write failed");
                #endif
            }
            file.close();
            result = true;
        }
        else
        {   // SD card has probably been removed
            sdFilesystemMounted = false;
            #ifdef SERIAL_OUT
            Serial.println("Failed to open file for writing");
            #endif
        }
     }
     return result;
}

uint64_t Filesystems::getSdTotalBytes()
{
    return sdFilesystemMounted ? sdFileSystem->totalBytes(): 0;
}

uint64_t Filesystems::getSdUsedBytes()
{
    return sdFilesystemMounted ? sdFileSystem->usedBytes(): 0;
}

File Filesystems::getLogDir()
{
    if(logDirReady)
    {
        return sdFileSystem->open(SARSATJRX_LOG_DIR);
    }
    else
    {
        return File();
    }
}

FS *Filesystems::spiFileSystem = &SPIFFS;

SDFS *Filesystems::sdFileSystem = &SD;

Filesystems *Filesystems::filesystemsInstance = nullptr;
