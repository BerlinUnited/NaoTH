#include "RobotInfo.h"

#include "Messages/Framework-Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;

void Serializer<RobotInfo>::serialize(const RobotInfo& representation, std::ostream& stream)
{
  naothmessages::RobotInfo info;

  info.set_platform(representation.platform);
  info.set_robotname(representation.robotName);

  info.set_headid(representation.headId);
  info.set_bodyid(representation.bodyId);
  info.set_bodynickname(representation.bodyNickName);

  info.set_basictimestep(representation.basicTimeStep);
  
  google::protobuf::io::OstreamOutputStream buf(&stream);
  info.SerializePartialToZeroCopyStream(&buf);
}

void Serializer<RobotInfo>::deserialize(std::istream& stream, RobotInfo& representation)
{
  naothmessages::RobotInfo info;
  google::protobuf::io::IstreamInputStream buf(&stream);
  info.ParseFromZeroCopyStream(&buf);

  if(info.has_platform()) {
    representation.platform = info.platform();
  }
  if(info.has_robotname()) {
    representation.robotName = info.robotname();
  }
  if(info.has_headid()) {
    representation.headId = info.headid();
  }
  if(info.has_bodyid()) {
    representation.bodyId = info.bodyid();
  }
  if(info.has_bodynickname()) {
    representation.bodyNickName = info.bodynickname();
  }
  if(info.has_basictimestep()) {
    representation.basicTimeStep = info.basictimestep();
  }

}

