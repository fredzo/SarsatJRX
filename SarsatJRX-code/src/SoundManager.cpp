#include <SoundManager.h>

SoundManager* SoundManager::soundManagerInstance = nullptr;

const char frameOkSoundString[]       = "line:d=4,o=5,b=300:32d6,32d#6,32e6,32f6,32f#6,32g6,32g#6,32a6";
const char frameKoSoundString[]       = "line:d=4,o=5,b=300:32a6,32g#6,32g6,32f#6,32f6,32e6,32d#6,32d6";
const char frameAnnoucenSoundString[] = "count:d=4,o=5,b=120:1e6";
const char touchSoundString[]         = "count:d=4,o=5,b=120:8e5";

void SoundManager::init()
{
  frameOkSound = MelodyFactory.loadRtttlString(frameOkSoundString);
  frameKoSound = MelodyFactory.loadRtttlString(frameKoSoundString);
  frameAnnoucenSound = MelodyFactory.loadRtttlString(frameAnnoucenSoundString);
  touchSound = MelodyFactory.loadRtttlString(touchSoundString);
}

void SoundManager::setVolume(byte volume)
{
  player->setVolume(volume);
}

void SoundManager::unmute()
{
  player->unmute();
}

void stopMelodyCallback()
{
  SoundManager::getSoundManager()->unmute();
}

void SoundManager::playFrameSound(bool error)
{
  if(error)
  {
      player->playAsync(frameKoSound);
  }
  else
  {
      player->playAsync(frameOkSound);
  }
}

void SoundManager::playFrameAnnounceSound()
{
    player->playAsync(frameAnnoucenSound);
}

void SoundManager::playTouchSound()
{
    player->playAsync(touchSound);
}