#ifndef FILESYSTEMS_H
#define FILESYSTEMS_H

#include <Arduino.h>
#include <FS.h>
#include <Beacon.h>
#include <SD.h>

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
        boolean saveBeacon(Beacon* beacon);

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

        static Filesystems *filesystemsInstance;
};

#endif 