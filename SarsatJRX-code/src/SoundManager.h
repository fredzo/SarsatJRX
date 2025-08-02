#ifndef MUSIC_MANAGER_H
#define MUSIC_MANAGER_H

#include <SarsatJRXConf.h>
#include <melody_player.h>
#include <melody_factory.h>


class SoundManager
{
    public :

        static SoundManager *getSoundManager()
        {
            if (soundManagerInstance == nullptr) {
                soundManagerInstance = new SoundManager();
            }
            return soundManagerInstance;
        }

        void init();

        void setVolume(byte volume);

        void playFrameSound(bool error);

        void playFrameAnnounceSound();

        void playTouchSound();

        void unmute();


    private :

        MelodyPlayer* player;

        Melody frameOkSound;
        Melody frameKoSound;
        Melody frameAnnoucenSound;
        Melody touchSound;


        SoundManager()
        {
            player = new MelodyPlayer(BUZZER_PIN,0,LOW);
        };

        ~SoundManager()
        {
            delete player;
        };

        static SoundManager *soundManagerInstance;
};

#endif 