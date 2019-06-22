#include "AudioControl.h"

using namespace naoth;

AudioControl::AudioControl() :
  capture(false),
  numChannels(4), // record all 4 channels by default
  sampleRate(8000),
  buffer_size(1024)
{}

void AudioControl::print(std::ostream& stream) const
{
  stream << "capture: " << (capture ? "on" : "off") << std::endl;
  stream << "numChannels: " << numChannels << std::endl;
  stream << "sampleRate: " << sampleRate << std::endl;
  stream << "buffer_size: " << buffer_size << std::endl;
}


