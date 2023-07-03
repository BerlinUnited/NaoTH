#include "AudioControl.h"

using namespace naoth;

AudioControl::AudioControl() : ParameterList("AudioControl")
{
  // NOTE: this is a request, rather than a parameter
  PARAMETER_REGISTER(capture) = false;

  PARAMETER_REGISTER(numChannels) = 4;     // record all 4 channels by default
  PARAMETER_REGISTER(sampleRate)  = 8000;
  PARAMETER_REGISTER(buffer_size) = 1024;
}

void AudioControl::print(std::ostream& stream) const
{
  stream << "capture: "     << (capture ? "on" : "off") << std::endl;
  stream << "numChannels: " << numChannels << std::endl;
  stream << "sampleRate: "  << sampleRate  << std::endl;
  stream << "buffer_size: " << buffer_size << std::endl;
}


