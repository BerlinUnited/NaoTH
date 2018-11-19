#include "AudioData.h"

#include "Messages/Framework-Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;

AudioData::AudioData():
  sampleRate(8000),
  numChannels(2),
  onlySoundInSet(true)
{
  
}

void AudioData::print(std::ostream& stream) const
{
  stream << "sampleRate: " << sampleRate << std::endl;
  stream << "onlySoundInSet: " << onlySoundInSet << std::endl;  
}

AudioData::~AudioData()
{
}