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


void Serializer<HeadMotionRequest>::serialize(const HeadMotionRequest& representation, std::ostream& stream)
{
  naothmessages::HeadMotionRequest message;
  message.set_id(representation.id);
  message.set_cameraid(representation.cameraID);
  message.mutable_targetjointposition()->set_x(representation.targetJointPosition.x);
  message.mutable_targetjointposition()->set_y(representation.targetJointPosition.y);
  message.mutable_targetpointinimage()->set_x(representation.targetPointInImage.x);
  message.mutable_targetpointinimage()->set_y(representation.targetPointInImage.y);
  message.mutable_targetpointintheworld()->set_x(representation.targetPointInTheWorld.x);
  message.mutable_targetpointintheworld()->set_y(representation.targetPointInTheWorld.y);
  message.mutable_targetpointintheworld()->set_z(representation.targetPointInTheWorld.z);
  message.mutable_searchcenter()->set_x(representation.searchCenter.x);
  message.mutable_searchcenter()->set_y(representation.searchCenter.y);
  message.mutable_searchcenter()->set_z(representation.searchCenter.z);
  message.mutable_searchsize()->set_x(representation.searchSize.x);
  message.mutable_searchsize()->set_y(representation.searchSize.y);
  message.mutable_searchsize()->set_z(representation.searchSize.z);
  message.set_searchdirection(representation.searchDirection);

  google::protobuf::io::OstreamOutputStream buf(&stream);
  message.SerializePartialToZeroCopyStream(&buf);
}

void Serializer<HeadMotionRequest>::deserialize(std::istream& stream, HeadMotionRequest& representation)
{
  naothmessages::HeadMotionRequest message;
  google::protobuf::io::IstreamInputStream buf(&stream);
  message.ParseFromZeroCopyStream(&buf);
  
  representation.id = static_cast<HeadMotionRequest::HeadMotionID>(message.id());
  representation.cameraID = static_cast<CameraInfo::CameraID>(message.cameraid());
  representation.targetJointPosition.x = message.mutable_targetjointposition()->x();
  representation.targetJointPosition.y = message.mutable_targetjointposition()->y();
  representation.targetPointInImage.x = message.mutable_targetpointinimage()->x();
  representation.targetPointInImage.y = message.mutable_targetpointinimage()->y();
  representation.targetPointInTheWorld.x = message.mutable_targetpointintheworld()->x();
  representation.targetPointInTheWorld.y = message.mutable_targetpointintheworld()->y();
  representation.targetPointInTheWorld.z = message.mutable_targetpointintheworld()->z();
  representation.searchCenter.x = message.mutable_searchcenter()->x();
  representation.searchCenter.y = message.mutable_searchcenter()->y();
  representation.searchCenter.z = message.mutable_searchcenter()->z();
  representation.searchSize.x = message.mutable_searchsize()->x();
  representation.searchSize.y = message.mutable_searchsize()->y();
  representation.searchSize.z = message.mutable_searchsize()->z();
  representation.searchDirection = message.searchdirection();
}

