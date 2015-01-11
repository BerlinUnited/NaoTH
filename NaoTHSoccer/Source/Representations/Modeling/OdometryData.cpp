/**
 * @file OdometryData.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 */

#include "OdometryData.h"
#include <Messages/Representations.pb.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;

void Serializer<OdometryData>::serialize(const OdometryData& representation, std::ostream& stream)
{
  naothmessages::OdometryData message;
  naothmessages::Pose2D* pose = message.mutable_pose();
  pose->mutable_translation()->set_x(representation.translation.x);
  pose->mutable_translation()->set_y(representation.translation.y);
  pose->set_rotation(representation.rotation);
  
  google::protobuf::io::OstreamOutputStream buf(&stream);
  message.SerializeToZeroCopyStream(&buf);
}

void Serializer<OdometryData>::deserialize(std::istream& stream, OdometryData& representation)
{
  naothmessages::OdometryData message;
  google::protobuf::io::IstreamInputStream buf(&stream);
  message.ParseFromZeroCopyStream(&buf);
  
  naothmessages::Pose2D* pose = message.mutable_pose();
  representation.translation.x = pose->mutable_translation()->x();
  representation.translation.y = pose->mutable_translation()->y();
  representation.rotation = pose->rotation();
}