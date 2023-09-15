#ifndef FILESYSTEMS_H
#define FILESYSTEMS_H

#include <Arduino.h>
#include <FS.h>

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
        FS* getSpiFilesystem() { return spiFileSystem; }
        FS* getSdFilesystem() { return sdFileSystem; }
        bool isSpiFilesystemMounted() { return spiFilesystemMounted ;}
        bool isSdFilesystemMounted() { return sdFilesystemMounted ;}

    private :

        Filesystems()
        {
        };

        ~Filesystems()
        {
        };
        
        static FS* spiFileSystem;
        bool spiFilesystemMounted = false;

        static FS* sdFileSystem;
        bool sdFilesystemMounted = false;

        static Filesystems *filesystemsInstance;
};

#endif 