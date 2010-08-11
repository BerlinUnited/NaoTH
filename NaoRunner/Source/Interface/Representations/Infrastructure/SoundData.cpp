#include "Representations/Infrastructure/SoundData.h"

SoundData::SoundData():
  mute(false),
  soundFile("")
{
  for(int i = 0; i < numOfSpeaker; i++)
  {
    volume[i] = 0;
  }
}

SoundData::~SoundData()
{
  
}
