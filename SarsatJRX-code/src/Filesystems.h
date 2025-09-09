#ifndef FILESYSTEMS_H
#define FILESYSTEMS_H

#include <Arduino.h>
#include <FS.h>
#include <Beacon.h>
#include <SD.h>
#include <vector>

class Filesystems
{
    public :
        static Filesystems *getFilesystems()
        {
            if (filesystemsInstance == nullptr) {
                filesystemsInstance = new Filesystems();
            }
            return filesystemsInstance;
        }

        void init();
        void initSdFs();
        void unmountSdFs();
        FS* getSpiFilesystem() { return spiFileSystem; }
        FS* getSdFilesystem() { return sdFileSystem; }
        bool isSpiFilesystemMounted() { return spiFilesystemMounted ;}
        bool isSdFilesystemMounted() { return sdFilesystemMounted ;}
        File getLogDir();
        uint64_t getSdTotalBytes();
        uint64_t getSdUsedBytes();
        bool isLogDirReady() { return logDirReady ;}
        bool saveBeacon(Beacon* beacon);
        bool loadBeacon(const char* fileName, volatile byte* frameBuffer);
        bool deleteBeacon(const char* fileName);
        int deleteAllBeacons();
        // Preferences load/save to sd card
        bool loadConfigFile(std::vector<String>& lines);
        bool saveConfigFile(const std::vector<String>& lines);

    private :

        Filesystems()
        {
        };

        ~Filesystems()
        {
        };
        
        static FS* spiFileSystem;
        bool spiFilesystemMounted = false;

        SPIClass* sdHandler;
        static SDFS* sdFileSystem;
        bool sdFilesystemMounted = false;
        bool logDirReady = false;
        char logDirPath[64];

        static Filesystems *filesystemsInstance;
};

#endif 