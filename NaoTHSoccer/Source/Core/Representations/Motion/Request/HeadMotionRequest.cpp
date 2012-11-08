/**
 * @file HeadMotionRequest.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 * Definition of HeadMotionReqeuest
 */


#include "HeadMotionRequest.h"

#include "Messages/Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <Tools/DataConversion.h>

using namespace naoth;

std::string HeadMotionRequest::getName(HeadMotionID id)
{
  switch (id)
  {
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

void HeadMotionRequest::print(ostream &stream) const
{
    stream << "Current MotionRequest = " << getName(id) << endl
           << " Selected Camera = " << cameraID << endl;
}

HeadMotionRequest::HeadMotionID HeadMotionRequest::getId(const std::string& name)
{
  for(int i = 0; i < numOfHeadMotion; i++)
  {
    if(name == getName((HeadMotionID)i)) return (HeadMotionID)i;
  }//end for
  
  return numOfHeadMotion;
}//end getId


void Serializer<HeadMotionRequest>::serialize(const HeadMotionRequest& representation, std::ostream& stream)
{
  naothmessages::HeadMotionRequest message;
  message.set_id(representation.id);
  message.set_cameraid(representation.cameraID);

  DataConversion::toMessage(representation.targetJointPosition, *(message.mutable_targetjointposition()));
  DataConversion::toMessage(representation.targetPointInImage, *(message.mutable_targetpointinimage()));
  DataConversion::toMessage(representation.targetPointInTheWorld, *(message.mutable_targetpointintheworld()));
  DataConversion::toMessage(representation.targetPointOnTheGround, *(message.mutable_targetpointontheground()));
  DataConversion::toMessage(representation.searchCenter, *(message.mutable_searchcenter()));
  DataConversion::toMessage(representation.searchSize, *(message.mutable_searchsize()));

  message.set_searchdirection(representation.searchDirection);

  message.set_coordinate(representation.coordinate);

  google::protobuf::io::OstreamOutputStream buf(&stream);
  message.SerializePartialToZeroCopyStream(&buf);
}//end serialize

void Serializer<HeadMotionRequest>::deserialize(std::istream& stream, HeadMotionRequest& representation)
{
  naothmessages::HeadMotionRequest message;
  google::protobuf::io::IstreamInputStream buf(&stream);
  message.ParseFromZeroCopyStream(&buf);
  
  representation.id = static_cast<HeadMotionRequest::HeadMotionID>(message.id());
  representation.cameraID = static_cast<CameraInfo::CameraID>(message.cameraid());

  DataConversion::fromMessage(message.targetjointposition(), representation.targetJointPosition);
  DataConversion::fromMessage(message.targetpointinimage(), representation.targetPointInImage);
  DataConversion::fromMessage(message.targetpointintheworld(), representation.targetPointInTheWorld);
  DataConversion::fromMessage(message.targetpointontheground(), representation.targetPointOnTheGround);
  DataConversion::fromMessage(message.searchcenter(), representation.searchCenter);
  DataConversion::fromMessage(message.searchsize(), representation.searchSize);

  representation.searchDirection = message.searchdirection();

  representation.coordinate = static_cast<HeadMotionRequest::Coordinate>(message.coordinate());
}//end deserialize


