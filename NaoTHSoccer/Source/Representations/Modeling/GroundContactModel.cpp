/**
 *  @file GroundContactModel.cpp
 *
 *  Created by Yigit Akcay on 11.01.17.
 */

#include "FootGroundContactDetector.h"

#include "Messages/Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;

void Serializer<GroundContactModel>::serialize(const GroundContactModel& representation, std::ostream& stream)
{
  naothmessages::GroundContactModel p;

  p.set_leftgroundcontact(representation.leftGroundContact);
  p.set_rightgroundcontact(representation.rightGroundContact);
  p.set_supportfoot(representation.supportFoot);
} // end serialize

void Serializer<GroundContactModel>::deserialize(std::istream& stream,GroundContactModel& representation)
{
  naothmessages::GroundContactModel p;
  google::protobuf::io::IstreamInputStream buf(&stream);
  p.ParseFromZeroCopyStream(&buf);

  if (p.has_leftgroundcontact())
  {
    representation.leftGroundContact = p.leftgroundcontact();
  }

  if (p.has_rightgroundcontact())
  {
    representation.rightGroundContact = p.rightgroundcontact();
  }

  if (p.has_supportfoot())
  {
    representation.supportFoot = p.supportfoot();
  }
} // end deserialize
