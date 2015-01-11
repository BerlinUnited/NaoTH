/**
* @file MotionStatus.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* 
*/

#include "MotionStatus.h"
#include <Messages/Representations.pb.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

using namespace naoth;

void Serializer<MotionStatus>::serialize(const MotionStatus& representation, std::ostream& stream)
{
  naothmessages::MotionStatus message;
  
  message.set_time(representation.time);
  message.set_lastmotion(representation.lastMotion);
  message.set_currentmotion(representation.currentMotion);
  message.set_headmotion(representation.headMotion);
  message.set_currentmotionstate(representation.currentMotionState);
  DataConversion::toMessage(representation.plannedMotion.lFoot, *message.mutable_plannedmotionleftfoot());
  DataConversion::toMessage(representation.plannedMotion.rFoot, *message.mutable_plannedmotionrightfoot());
  DataConversion::toMessage(representation.plannedMotion.hip, *message.mutable_plannedmotionhip());

  // step control
  naothmessages::StepControlStatus* stepControlStatus =  message.mutable_stepcontrolstatus();
  stepControlStatus->set_stepid(representation.stepControl.stepID);
  stepControlStatus->set_moveablefoot(representation.stepControl.moveableFoot);

  google::protobuf::io::OstreamOutputStream buf(&stream);
  message.SerializeToZeroCopyStream(&buf);
}

void Serializer<MotionStatus>::deserialize(std::istream& stream, MotionStatus& representation)
{
  naothmessages::MotionStatus message;
  google::protobuf::io::IstreamInputStream buf(&stream);
  if(message.ParseFromZeroCopyStream(&buf))
  {
    representation.time = message.time();
    representation.lastMotion = static_cast<motion::MotionID>(message.lastmotion());
    representation.currentMotion = static_cast<motion::MotionID>(message.currentmotion());
    representation.headMotion = static_cast<HeadMotionRequest::HeadMotionID>(message.headmotion());
    representation.currentMotionState = static_cast<motion::State>(message.currentmotionstate());
    DataConversion::fromMessage(message.plannedmotionleftfoot(), representation.plannedMotion.lFoot);
    DataConversion::fromMessage(message.plannedmotionrightfoot(), representation.plannedMotion.rFoot);
    DataConversion::fromMessage(message.plannedmotionhip(), representation.plannedMotion.hip);

    // step control
    const naothmessages::StepControlStatus& stepControlStatus =  message.stepcontrolstatus();
    representation.stepControl.stepID = stepControlStatus.stepid();
    representation.stepControl.moveableFoot = static_cast<MotionStatus::StepControlStatus::MoveableFoot>(stepControlStatus.moveablefoot());
  }
  else
  {
    THROW("Serializer<MotionStatus>::deserialize failed");
  }
}
