/**
 * @file HeadMotionRequest.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * Definition of HeadMotionReqeuest
 */


#include "HeadMotionRequest.h"

#include "Messages/Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;

string HeadMotionRequest::getName() const
{
  switch (id) {
  case search: return "search";
  case goto_angle: return "goto_angle";
  case look_at_point: return "look_at_point";
  case hold: return "hold";
  case stabilize: return "stabilize";
  case look_straight_ahead: return "look_straight_ahead";
  case look_at_world_point: return "look_at_world_point";
  case look_at_point_on_the_ground: return "look_at_point_on_the_ground";
  default: return "unknown";
  }///end switch
}///end getName

  /*
void Serializer<HeadMotionRequest>::serialize(const HeadMotionRequest& representation, std::ostream& stream)
{
  naothmessages::HeadMotionRequest r;
  r.set_id(representation.id);
  r.set_cameraid(representation.cameraID);
  r.mutable_targetjointposition()->set_x(representation.targetJointPosition.x);
  r.mutable_targetjointposition()->set_y(representation.targetJointPosition.y);
  r.targetPointInImage = representation.targetPosition;
  r.targetPointInTheWorld = representation.targetPointInTheWorld;
  r.searchCenter = representation.searchCenter;
  r.searchSize = representation.searchSize;
  r.searchDirection = representation.searchDirection;

  google::protobuf::io::OstreamOutputStream buf(&stream);
  r.SerializePartialToZeroCopyStream(&buf);
}

void Serializer<HeadMotionRequest>::deserialize(std::istream& stream, HeadMotionRequest& representation)
{

  naothmessages::FrameInfo r;
  google::protobuf::io::IstreamInputStream buf(&stream);
  r.ParseFromZeroCopyStream(&buf);


//  representation.time = f.time();
//  representation.frameNumber = f.framenumber();
}
   */

