#include <Filesystems.h>
#include <SarsatJRXConf.h>
#include <SPIFFS.h>
#include <SD.h>

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
    }
    else
    {
        #ifdef SERIAL_OUT
        Serial.println("Could not  mount SD Card");
        #endif
        sdFilesystemMounted = false;
    }
}

FS *Filesystems::spiFileSystem = &SPIFFS;

FS *Filesystems::sdFileSystem = &SD;

Filesystems *Filesystems::filesystemsInstance = nullptr;
