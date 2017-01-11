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

  p.set_leftgroundcontactaverage(representation.leftGroundContactAverage);
  p.set_rightgroundcontactaverage(representation.rightGroundContactAverage);
}
