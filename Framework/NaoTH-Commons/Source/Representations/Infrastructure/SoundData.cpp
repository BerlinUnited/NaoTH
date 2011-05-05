#include "Representations/Infrastructure/SoundData.h"

using namespace naoth;

SoundData::SoundData()
:
  mute(false),
  soundFile(""),
  snd_ctl_dump(""),
  period_size(0),
  periods(0),
  rate(0),
  channels(0)
{}

SoundData::~SoundData()
{}

SoundPlayData::SoundPlayData()
{
  for(int i = 0; i < numOfSpeaker; i++)
  {
    volume[i] = 0;
  }
}

SoundPlayData::~SoundPlayData()
{}

void SoundPlayData::print(std::ostream& stream) const
{
  stream << "file to play: " << soundFile << endl
          << "period size: " << period_size << endl
          << "period count: " << periods << endl
          << "channels: " << channels << endl
          << "sample rate: " << rate << endl
          << "ALSA state" << endl << snd_ctl_dump << endl;
}

SoundCaptureData::SoundCaptureData()
:
  lastBytesCaptured(0)
{
  for(int i = 0; i < numOfMicrophone; i++)
  {
    volume[i] = 0;
  }
}

SoundCaptureData::~SoundCaptureData()
{}

void SoundCaptureData::print(ostream& stream) const
{
  stream << "file to play: " << soundFile << endl
          << "period size: " << period_size << endl
          << "period count: " << periods << endl
          << "channels: " << channels << endl
          << "sample rate: " << rate << endl
          << "ALSA state" << endl << snd_ctl_dump << endl;
}