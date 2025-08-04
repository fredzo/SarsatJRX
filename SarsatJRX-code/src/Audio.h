#ifndef RADIO_H
#define RADIO_H

#include <Arduino.h>

class Audio
{
    public :
        static Audio *getAudio()
        {
            if (audioInstance == nullptr) {
                audioInstance = new Audio();
            }
            return audioInstance;
        }

        void audioInit();
        void audioStop();

        int getSignalPower();

        bool isDiscriInput();

        bool discriHasChanged();

        // Audio task processing
        void handleTask();

    private :
        Audio()
        {
        };

        ~Audio()
        {
        };

        long filterAccumulator = 0;
        int signalPower = 0;
        int noiseFloor = 0;

        // Last discri sample time
        unsigned long lastDiscriSampleTime = 0;
        bool discriState = false;
        bool discriChanged = false;

        // Static members
        static Audio *audioInstance;
};

#endif 