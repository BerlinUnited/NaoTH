
#include "OptiTrackData.h"

#include "Messages/Framework-Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <Tools/DataConversion.h>

using namespace naoth;

void Serializer<OptiTrackData>::serialize(const OptiTrackData& representation, std::ostream& stream)
{
  naothmessages::OptiTrackData message;
  
  for(const auto& element : representation.trackables) {
    naothmessages::OptiTrackData::TrackableItem* s = message.add_trackables();
    s->set_name(element.first);
    DataConversion::toMessage(element.second, *(s->mutable_pose()));
  }

  google::protobuf::io::OstreamOutputStream buf(&stream);
  message.SerializeToZeroCopyStream(&buf);
}

void Serializer<OptiTrackData>::deserialize(std::istream& stream, OptiTrackData& representation)
{
  naothmessages::OptiTrackData message;
  google::protobuf::io::IstreamInputStream buf(&stream);
  message.ParseFromZeroCopyStream(&buf);

  representation.trackables.clear();
  for(int i = 0; i < message.trackables().size(); ++i) {
    const naothmessages::OptiTrackData::TrackableItem& s = message.trackables(i);
    Pose3D pose;
    DataConversion::fromMessage(s.pose(), pose);
    representation.trackables.insert(std::make_pair(s.name(), pose));
  }
}