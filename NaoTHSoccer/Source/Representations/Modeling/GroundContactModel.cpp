/**
 *  @file GroundContactModel.cpp
 *
 *  Created by Yigit Akcay on 11.01.17.
 */

#include "GroundContactModel.h"

#include "Messages/Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;

void Serializer<GroundContactModel>::serialize(const GroundContactModel& representation, std::ostream& stream)
{
  naothmessages::GroundContactModel message;
  google::protobuf::io::OstreamOutputStream buf(&stream);

  message.set_leftgroundcontact(representation.leftGroundContact);
  message.set_rightgroundcontact(representation.rightGroundContact);

  message.set_supportfoot((naothmessages::GroundContactModel_Foot)representation.supportFoot);

  message.SerializePartialToZeroCopyStream(&buf);
} // end serialize

void Serializer<GroundContactModel>::deserialize(std::istream& stream,GroundContactModel& representation)
{
  naothmessages::GroundContactModel message;
  google::protobuf::io::IstreamInputStream buf(&stream);
  message.ParseFromZeroCopyStream(&buf);

  representation.leftGroundContact = message.leftgroundcontact();
  representation.rightGroundContact = message.rightgroundcontact();

  representation.supportFoot = (GroundContactModel::Foot)message.supportfoot();
}// end deserialize
