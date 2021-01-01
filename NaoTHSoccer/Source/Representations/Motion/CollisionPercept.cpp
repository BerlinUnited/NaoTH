/**
* @file CollisionPercept.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* 
*/

#include "CollisionPercept.h"
#include <Messages/Representations.pb.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;

void Serializer<CollisionPercept>::serialize(const CollisionPercept& representation, std::ostream& stream)
{
  naothmessages::CollisionPercept message;
  
  message.set_timecollisionarmleft(representation.timeCollisionArmLeft);
  message.set_timecollisionarmright(representation.timeCollisionArmRight);

  message.set_iscollisionleftbumper(representation.collision_left_bumper);
  message.set_iscollisionrightbumper(representation.collision_right_bumper);
  message.set_lastcomputedcollisionleft(representation.lastComputedCollisionLeft);
  message.set_lastcomputedcollisionright(representation.lastComputedCollisionRight);

  google::protobuf::io::OstreamOutputStream buf(&stream);
  message.SerializeToZeroCopyStream(&buf);
}

void Serializer<CollisionPercept>::deserialize(std::istream& stream, CollisionPercept& representation)
{
  naothmessages::CollisionPercept message;
  google::protobuf::io::IstreamInputStream buf(&stream);
  if(message.ParseFromZeroCopyStream(&buf))
  {
    representation.timeCollisionArmLeft = message.timecollisionarmleft();
    representation.timeCollisionArmRight = message.timecollisionarmright();
    representation.collision_left_bumper = message.iscollisionleftbumper();
    representation.collision_left_bumper = message.iscollisionrightbumper();
    representation.lastComputedCollisionLeft = message.lastcomputedcollisionleft();
    representation.lastComputedCollisionRight = message.lastcomputedcollisionright();
  }
  else
  {
    THROW("Serializer<CollisionPercept>::deserialize failed");
  }
}
