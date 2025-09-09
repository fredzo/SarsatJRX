#include <Filesystems.h>
#include <SarsatJRXConf.h>
#include <SPIFFS.h>
#include <Util.h>
#include <RTC.h>

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
    sdHandler = new SPIClass(HSPI);
    pinMode(SD_MISO, INPUT_PULLUP);
    sdHandler->begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);
    initSdFs();
}

void Filesystems::initSdFs()
{   // Start SD card
    if(!sdFilesystemMounted)
    {   // Since our hardware is what it is, we don't use default 4MHz frequency but a lower value
        if(SD.begin(SD_CS,*sdHandler,1000000))
        {
            sdFilesystemMounted = true;
            // Create SarsatJRX folder if needed
            logDirReady = false;
            if(SD.mkdir(SARSATJRX_LOG_DIR_NAME))
            {   // Get current date to setup log dir path
                Rtc::Date today = Rtc::getRtc()->getDate();
                snprintf(logDirPath,sizeof(logDirPath),SARSATJRX_LOG_DIR_PATH_TEMPLATE,SARSATJRX_LOG_DIR_NAME,today.day,today.month,today.year);
                // Create folder for today
                if(SD.mkdir(logDirPath))
                {
                    logDirReady = true;
                }
            }
            if(!logDirReady)
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
}

void Filesystems::unmountSdFs()
{   // Unmount SD card
    if(sdFilesystemMounted)
    {   
        SD.end();
        sdFilesystemMounted = false;
        logDirReady = false;
    }
}

bool Filesystems::saveBeacon(Beacon* beacon)
{
    bool result = false;
    if(sdFilesystemMounted&&logDirReady)
    {
        char buffer[64];
        snprintf(buffer,sizeof(buffer),LOG_FILENAME_TEMPLATE,logDirPath,beacon->date.day,beacon->date.month,beacon->date.year,beacon->date.hour,beacon->date.minute,beacon->date.second);
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

bool Filesystems::loadBeacon(const char* fileName, volatile byte* frameBuffer)
{
    bool result = false;
    if(sdFilesystemMounted&&logDirReady)
    {
        char buffer[64];
        snprintf(buffer,sizeof(buffer),"%s/%s",logDirPath,fileName);
        File file = sdFileSystem->open(buffer, FILE_READ);
        if(file)
        {
            char* charBuffer[64];
            // Try and read a long frame (2 char per bytes (hex format))
            int readBytes = file.read((byte*)charBuffer,18*2);
            if(readBytes > 0)
            {   
                charBuffer[readBytes]=0; // String terminator
                // Convert hex string to bytes
                readHexString(frameBuffer,String((const char*)charBuffer));
                file.close();
                result = true;
            }
            else
            {   // File not found or empty
                #ifdef SERIAL_OUT
                Serial.printf("Could not read beacon file '%s'\n",buffer);
                #endif
            }
        }
        else
        {   // SD card has probably been removed
            sdFilesystemMounted = false;
            #ifdef SERIAL_OUT
            Serial.printf("Failed to open beacon file '%s'\n",buffer);
            #endif
        }
     }
     return result;
}

bool Filesystems::deleteBeacon(const char* fileName)
{
    bool result = false;
    if(sdFilesystemMounted&&logDirReady)
    {
        char buffer[64];
        snprintf(buffer,sizeof(buffer),"%s/%s",logDirPath,fileName);
        result = sdFileSystem->remove(buffer);
    }
    return result;
}

int Filesystems::deleteAllBeacons()
{
    int result = 0;
    if(sdFilesystemMounted&&logDirReady)
    {
        File logDir = getLogDir();
        if(logDir && logDir.isDirectory())
        {
            File beacon = logDir.openNextFile();
            while(beacon)
            {
                if(!beacon.isDirectory())
                {
                    String name = beacon.name();
                    if(name.endsWith(LOG_FILE_EXTENSION))
                    {
                        char buffer[64];
                        snprintf(buffer,sizeof(buffer),"%s/%s",logDirPath,name.c_str());
                        if(sdFileSystem->remove(buffer))
                        {
                            result++;
                        }
                    }
                }
                beacon = logDir.openNextFile();
            }
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
        return sdFileSystem->open(logDirPath);
    }
    else
    {
        return File();
    }
}

bool Filesystems::loadConfigFile(std::vector<String>& lines)
{
    if(!sdFilesystemMounted) return false;
    if(sdFileSystem->exists(SARSATJRX_CONFIG_FILENAME))
    {
        File configFile = sdFileSystem->open(SARSATJRX_CONFIG_FILENAME);
        if(!configFile)
        {
            Serial.printf("Could not open config file '%s' (file does not exist)\n",SARSATJRX_CONFIG_FILENAME);
            return false;
        }
        if(configFile.isDirectory())
        {
            Serial.printf("Could not open config file '%s' (it's an directory)\n",SARSATJRX_CONFIG_FILENAME);
            return false;
        }
        while (configFile.available()) 
        {
            String line = configFile.readStringUntil('\n');
            line.trim();
            if(!line.isEmpty())
            {
                lines.push_back(line);
                //Serial.printf("Read conf line:%s\n",line.c_str());
            }
        }
        configFile.close();
        return true;
    }
    else
    {
        Serial.printf("Could not open config file '%s' (file does not exist)\n",SARSATJRX_CONFIG_FILENAME);
        return false;
    }
}

bool Filesystems::saveConfigFile(const std::vector<String>& lines)
{
    if(!sdFilesystemMounted) return false;
    File configFile = sdFileSystem->open(SARSATJRX_CONFIG_FILENAME, FILE_READ);
    if(configFile)
    {   // Remove previously existing file
        sdFileSystem->remove(SARSATJRX_CONFIG_FILENAME);
    }
    // (Re)create the file
    configFile = sdFileSystem->open(SARSATJRX_CONFIG_FILENAME, FILE_WRITE, true);
    if(!configFile)
    {
        Serial.printf("Could not open config file '%s'\n",SARSATJRX_CONFIG_FILENAME);
        return false;
    }
    configFile.seek(0);
    for (auto line : lines) 
    {   // Save lines
        configFile.println(line);
    }
    configFile.close();
    //Serial.println("Config file saved !");
    return true;
}


FS *Filesystems::spiFileSystem = &SPIFFS;

SDFS *Filesystems::sdFileSystem = &SD;

Filesystems *Filesystems::filesystemsInstance = nullptr;
