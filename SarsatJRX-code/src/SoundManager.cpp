#include <SoundManager.h>

SoundManager* SoundManager::soundManagerInstance = nullptr;

const char frameOkSoundString[]       = "ok:d=4,o=5,b=300:32d6,32d#6,32e6,32f6,32f#6,32g6,32g#6,32a6";
const char frameKoSoundString[]       = "ko:d=4,o=5,b=300:32a6,32g#6,32g6,32f#6,32f6,32e6,32d#6,32d6";
const char countDownLowString[]       = "countl:d=16,o=5,b=60:a";
const char countDownHighString[]      = "counth:d=16,o=5,b=60:a6";
const char touchSoundString[]         = "touch:d=4,o=5,b=300:32a8";

void SoundManager::init()
{
  frameOkSound = MelodyFactory.loadRtttlString(frameOkSoundString);
  frameKoSound = MelodyFactory.loadRtttlString(frameKoSoundString);
  countDownLowSound  = MelodyFactory.loadRtttlString(countDownLowString);
  countDownHighSound = MelodyFactory.loadRtttlString(countDownHighString);
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

void SoundManager::playCountDownLowSound()
{
    player->playAsync(countDownLowSound);
}

void SoundManager::playCountDownHighSound()
{
    player->playAsync(countDownHighSound);
}

void SoundManager::playTouchSound()
{
    player->playAsync(touchSound);
}