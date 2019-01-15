/**
* @file MotionRequest.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
*/

#include "MotionRequest.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;

void Serializer<MotionRequest>::serialize(const MotionRequest& representation, std::ostream& stream)
{
  naothmessages::MotionRequest message;
  message.set_id(representation.id);
  message.set_forced(representation.forced);
  message.set_time(representation.time);
  message.set_cognitionframenumber(representation.cognitionFrameNumber);
  message.set_starndardstand(representation.standardStand);
  message.set_disable_relaxed_stand(representation.disable_relaxed_stand);
  message.set_calibratefoottouchdetector(representation.calibrateFootTouchDetector);
  switch (representation.id)
  {
  case motion::walk:
    Serializer<WalkRequest>::serialize(representation.walkRequest, message.mutable_walkrequest());
    break;
  case motion::kick:
    Serializer<KickRequest>::serialize(representation.kickRequest, message.mutable_kickrequest());
    break;
  case motion::stand:
    message.set_standheight(representation.standHeight);
  default:
    //TODO
    break;
  }

  //
  Serializer<GraspRequest>::serialize(representation.graspRequest, message.mutable_grasprequest());

  Serializer<ArmMotionRequest>::serialize(representation.armMotionRequest, message.mutable_armmotionrequest());
 
  google::protobuf::io::OstreamOutputStream buf(&stream);
  message.SerializeToZeroCopyStream(&buf);
}

void Serializer<MotionRequest>::deserialize(std::istream& stream, MotionRequest& representation)
{
  naothmessages::MotionRequest message;
  google::protobuf::io::IstreamInputStream buf(&stream);
  message.ParseFromZeroCopyStream(&buf);

  representation.id = static_cast<motion::MotionID>(message.id());
  representation.forced = message.forced();
  representation.time = message.time();
  representation.standardStand = message.starndardstand();
  representation.disable_relaxed_stand = message.disable_relaxed_stand();
  if(message.has_calibratefoottouchdetector())
  {
    representation.calibrateFootTouchDetector = message.calibratefoottouchdetector();
  }
  if ( message.has_walkrequest() )
  {
    Serializer<WalkRequest>::deserialize(&(message.walkrequest()), representation.walkRequest);
  }
  if ( message.has_kickrequest() )
  {
    Serializer<KickRequest>::deserialize(&(message.kickrequest()), representation.kickRequest);
  }
  if ( message.has_standheight() )
  {
    representation.standHeight = message.standheight();
  }
  if(message.has_cognitionframenumber())
  {
    representation.cognitionFrameNumber = message.cognitionframenumber();
  }
  if(message.has_grasprequest())
  {
    Serializer<GraspRequest>::deserialize(&(message.grasprequest()), representation.graspRequest);
  }
  if(message.has_armmotionrequest())
  {
    Serializer<ArmMotionRequest>::deserialize(&(message.armmotionrequest()), representation.armMotionRequest);
  }
}
