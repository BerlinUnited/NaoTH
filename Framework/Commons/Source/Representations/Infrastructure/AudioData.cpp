#include "AudioData.h"

#include "Messages/AudioData.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include <algorithm>

using namespace naoth;

AudioData::AudioData():
  sampleRate(8000),
  numChannels(2),
  timestamp(0)
{ 
}

void AudioData::print(std::ostream& stream) const
{
  stream << "sampleRate: " << sampleRate << std::endl;
  stream << "numChannels: " << numChannels << std::endl;
  stream << "Number of Samples: " << samples.size() << std::endl;
  stream << "timestamp: " << timestamp << std::endl;
}

void Serializer<AudioData>::serialize(const AudioData& representation, std::ostream& stream)
{
  // the data has to be converted to a YUV (1 byte for each) array. no interlacing
  naothmessages::AudioData msg;

  msg.set_samplerate(representation.sampleRate);
  msg.set_numchannels(representation.numChannels);
  msg.set_samplesize(sizeof(short));
  msg.set_timestamp(representation.timestamp);
  msg.set_samples((unsigned char*)representation.samples.data(), representation.samples.size()*sizeof(short));
  google::protobuf::io::OstreamOutputStream buf(&stream);
  msg.SerializeToZeroCopyStream(&buf);
}

void Serializer<AudioData>::deserialize(std::istream& stream, AudioData& representation)
{
  naothmessages::AudioData msg;

  google::protobuf::io::IstreamInputStream buf(&stream);
  msg.ParseFromZeroCopyStream(&buf);

  representation.sampleRate = msg.samplerate();
  representation.numChannels = msg.numchannels();
  representation.timestamp = msg.timestamp();

  assert(msg.samplesize() == sizeof(uint16_t));
  representation.samples.resize(msg.samples().size() / sizeof(uint16_t));
  std::copy_n(msg.samples().data(), msg.samples().size(), (unsigned char*)representation.samples.data());
}