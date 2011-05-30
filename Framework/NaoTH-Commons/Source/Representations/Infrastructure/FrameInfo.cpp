#include "FrameInfo.h"

#include "Messages/Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;

void Serializer<FrameInfo>::serialize(const FrameInfo& representation, std::ostream& stream)
{
  naothmessages::FrameInfo f;
  f.set_framenumber(representation.frameNumber);
  f.set_time(representation.time);
  f.set_basictimestep(representation.basicTimeStep);

  google::protobuf::io::OstreamOutputStream buf(&stream);
  f.SerializePartialToZeroCopyStream(&buf);
}

void Serializer<FrameInfo>::deserialize(std::istream& stream, FrameInfo& representation)
{
  naothmessages::FrameInfo f;
  google::protobuf::io::IstreamInputStream buf(&stream);
  f.ParseFromZeroCopyStream(&buf);

  representation.time = f.time();
  representation.frameNumber = f.framenumber();
  representation.basicTimeStep = f.basictimestep();
}

